#pragma once

#include <Core.hpp>
#include <Uuid.hpp>
#include <Alghorithm.hpp>
#include <memory>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <algorithm>
#include <cstdint>

#include "WindowInterface/IWindow.hpp"
#include "Utils/Windows/WindowPositionQueue.hpp"


namespace Temp
{
    // ---------------- Percent ----------------
    class Percent
    {
        uint8_t value; // 0..100
    public:
        explicit Percent(uint8_t v) : value(v) { assert(v <= 100); }

        float toFactor() const { return value / 100.0f; }
        int toPixels(int total) const { return static_cast<int>(total * toFactor()); }

        uint8_t get() const { return value; }
    };

    // ---------------- DockNode ----------------
    enum class DockNodeType
    {
        ROOT,
        SPLIT,
        TAB_GROUP,
        TAB,
        FLOATING
    };

    class DockNode : public std::enable_shared_from_this<DockNode>
    {
    public:
        virtual ~DockNode() = default;
        virtual DockNodeType getType() const = 0;
        virtual void updateLayout(const NbRect<int>& newRect) = 0;
        virtual bool canAcceptChild(const std::shared_ptr<DockNode>& child) const = 0;

        const nbstl::Uuid getUuid() const noexcept { return uuid; }
        std::shared_ptr<DockNode> getParent() const { return parent.lock(); }
        const NbRect<int>& getRect() const { return rect; }

        void setParent(const std::shared_ptr<DockNode>& node) noexcept
        {
            parent = node;
        }

        void addChildren(const std::shared_ptr<DockNode>& child) noexcept
        {
            children.push_back(child);
        }

        std::vector<std::shared_ptr<DockNode>>& getChildren() { return children; };

    protected:
        nbstl::Uuid uuid;
        std::weak_ptr<DockNode> parent;
        std::vector<std::shared_ptr<DockNode>> children;
        NbRect<int> rect;
    };

    // ---------------- RootNode ----------------
    class RootNode : public DockNode
    {
    public:
        DockNodeType getType() const override { return DockNodeType::ROOT; }

        bool canAcceptChild(const std::shared_ptr<DockNode>& child) const override
        {
            return child->getType() == DockNodeType::SPLIT || child->getType() == DockNodeType::TAB_GROUP;
        }

        void updateLayout(const NbRect<int>& newRect) override
        {
            rect = newRect;
            for (auto& child : children)
                child->updateLayout(newRect);
        }

        void addChild(const std::shared_ptr<DockNode>& child)
        {
            if (canAcceptChild(child))
            {
                child->setParent(shared_from_this());
                children.push_back(child);
            }
        }

    private:
        std::string layoutVersion;
        
    };

    // ---------------- SplitNode ----------------
    class SplitNode : public DockNode
    {
    public:
        enum class Orientation { HORIZONTAL, VERTICAL };

        SplitNode(Orientation o) : orientation(o), minSizeInPixel(20) {}

        DockNodeType getType() const override { return DockNodeType::SPLIT; }

        bool canAcceptChild(const std::shared_ptr<DockNode>& child) const override
        {
            return child->getType() != DockNodeType::ROOT && child->getType() != DockNodeType::FLOATING;
        }

        void updateLayout(const NbRect<int>& newRect) override
        {
            rect = newRect;
            recalcChildLayouts();
        }

        void addChild(const std::shared_ptr<DockNode>& child, Percent sizePercent)
        {
            if (!canAcceptChild(child)) return;
            child->setParent(shared_from_this());
            children.push_back(child);
            sizes.push_back(sizePercent);
            recalcChildLayouts(); // сразу пересчитываем
        }

        // --- публичный метод для пересчета layout извне ---
        void recalculateLayout()
        {
            recalcChildLayouts();
        }

    private:
        void recalcChildLayouts()
        {
            if (children.empty() || sizes.size() != children.size())
                return;

            int total = (orientation == Orientation::HORIZONTAL) ? rect.width : rect.height;
            int accumulated = 0;

            for (size_t i = 0; i < children.size(); ++i)
            {
                NbRect<int> childRect = rect;
                int sizePx = static_cast<int>(total * sizes[i].toFactor());

                if (orientation == Orientation::HORIZONTAL)
                {
                    childRect.x += accumulated;
                    childRect.width = sizePx;
                }
                else
                {
                    childRect.y += accumulated;
                    childRect.height = sizePx;
                }

                // рекурсивно обновляем layout дочернего узла
                children[i]->updateLayout(childRect);
                accumulated += sizePx;
            }
        }


    private:
        Orientation orientation;
        std::vector<Percent> sizes;
        uint16_t minSizeInPixel;
    };

    // ---------------- TabNode ----------------
    class TabGroupNode; // Forward

    class TabNode : public DockNode
    {
    public:
        DockNodeType getType() const override { return DockNodeType::TAB; }
        bool canAcceptChild(const std::shared_ptr<DockNode>&) const override { return false; }

        void updateLayout(const NbRect<int>& newRect) override
        {
            rect = newRect;
            if (window)
                window->setClientRect(rect);
        }

        void setTabGroup(const std::shared_ptr<DockNode>& group)
        {
            if (group->getType() != DockNodeType::TAB_GROUP)
            {
                return;
            }

            tabGroup = std::dynamic_pointer_cast<TabGroupNode>(group);
            parent = group;
        }

        void setTitle(const std::string& newTitle) noexcept { title = newTitle; }

        void setWindow(std::shared_ptr<WindowInterface::IWindow> win) { window = win; }
        std::shared_ptr<WindowInterface::IWindow> getWindow() noexcept { return window; }

    private:
        std::weak_ptr<TabGroupNode> tabGroup;
        std::shared_ptr<WindowInterface::IWindow> window; // <-- тут хранится UI окно
        std::string title;
        uint8_t tabFlags = 0;
    };

    // ---------------- TabGroupNode ----------------
    class TabGroupNode : public DockNode
    {
    public:
        DockNodeType getType() const override { return DockNodeType::TAB_GROUP; }

        bool canAcceptChild(const std::shared_ptr<DockNode>& child) const override
        {
            return child->getType() == DockNodeType::TAB;
        }

        void updateLayout(const NbRect<int>& newRect) override
        {
            rect = newRect;

            for (size_t i = 0; i < tabs.size(); ++i)
            {
                auto& tab = tabs[i];
                if (i == activeTabIndex)
                    tab->updateLayout(rect);  // активная вкладка занимает весь rect
                else
                    tab->updateLayout({ 0,0,0,0 }); // неактивная — ноль
            }
        }

        void addTab(const std::shared_ptr<TabNode>& tab)
        {
            if (!canAcceptChild(tab)) return;
            tab->setTabGroup(shared_from_this());
            tabs.push_back(tab);
            children.push_back(tab); // унифицированное хранение
            updateLayout(rect);
        }

        void setActiveTab(size_t index)
        {
            if (index < tabs.size())
            {
                activeTabIndex = index;
                updateLayout(rect);
            }
        }

    private:
        size_t activeTabIndex = 0;
        std::vector<std::shared_ptr<TabNode>> tabs;
    };

    // ---------------- FloatingNode ----------------
    class FloatingNode : public DockNode
    {
    public:
        DockNodeType getType() const override { return DockNodeType::FLOATING; }

        bool canAcceptChild(const std::shared_ptr<DockNode>& child) const override
        {
            return child->getType() == DockNodeType::SPLIT || child->getType() == DockNodeType::TAB_GROUP;
        }

        void updateLayout(const NbRect<int>& newRect) override
        {
            rect = newRect;
            if (window)
                window->setClientRect(rect);

            for (auto& child : children)
                child->updateLayout(newRect);
        }

        void setWindow(std::shared_ptr<WindowInterface::IWindow> win) { window = win; }
        std::shared_ptr<WindowInterface::IWindow> getWindow() { return window; }

    private:
        std::shared_ptr<WindowInterface::IWindow> window;
        uint32_t windowId;
        bool isDocked = false;
        NbRect<int> restoreRect;
    };


    // ---------------- DockTreeManager ----------------
    class DockTreeManager
    {
    public:
        DockTreeManager() = default;

        void setRoot(const std::shared_ptr<RootNode>& r) { root = r; }

        std::shared_ptr<DockNode> findNode(const nbstl::Uuid& id)
        {
            auto it = registry.find(id);
            if (it != registry.end()) return it->second.lock();
            return nullptr;
        }

        void registerNode(const std::shared_ptr<DockNode>& node)
        {
            registry[node->getUuid()] = node;
        }

        std::shared_ptr<SplitNode> createSplit(const std::shared_ptr<DockNode>& parent, SplitNode::Orientation orientation)
        {
            if (!parent || !parent->canAcceptChild(nullptr)) return nullptr;
            auto split = std::make_shared<SplitNode>(orientation);
            split->setParent(parent);
            parent->addChildren(split);
            registerNode(split);
            return split;
        }

        std::unordered_map<nbstl::Uuid, std::weak_ptr<DockNode>>& getRegistry() { return registry; }
        

        void optimizeTree(); // TODO: удаление пустых Split/TabGroup

    private:
        std::shared_ptr<RootNode> root;
        std::unordered_map<nbstl::Uuid, std::weak_ptr<DockNode>> registry;
    };


    enum class DockPosition
    {
        LEFT,
        RIGHT,
        TOP,
        BOTTOM,
        TAB
    };

    class DockingSystem
    {
    public:
        DockingSystem(std::shared_ptr<WindowInterface::IWindow> mainWindow)
            : mainWindow(mainWindow)
        {
            // RootNode создаём
            rootNode = std::make_shared<RootNode>();

            // Менеджер
            manager = std::make_shared<DockTreeManager>();
            manager->setRoot(rootNode);
            manager->registerNode(rootNode);

            // Обновление layout
            NbRect<int> rect{ 0, 0, mainWindow->getWidth(), mainWindow->getHeight() };
            rootNode->updateLayout(rect);
        }


        // --- Доковать как вкладку ---
        std::shared_ptr<TabNode> dockAsTab(std::shared_ptr<WindowInterface::IWindow> window,
            std::shared_ptr<WindowInterface::IWindow> targetWindow = nullptr,
            const std::string& title = "")
        {
            if (!window) return nullptr;

            // Ищем targetNode
            auto targetNode = findNodeByWindow(targetWindow);
            if (!targetNode)
            {
                // Если не найдено, добавляем в первый Split/TabGroup под root
                targetNode = rootNode->getChildren().empty() ? nullptr : rootNode->getChildren()[0];
                if (!targetNode)
                {
                    // Создаем TabGroup под root
                    auto tabGroup = std::make_shared<TabGroupNode>();
                    rootNode->addChild(tabGroup);
                    manager->registerNode(tabGroup);
                    targetNode = tabGroup;
                }
            }

            std::shared_ptr<TabGroupNode> group;

            if (targetNode->getType() == DockNodeType::TAB_GROUP)
                group = std::dynamic_pointer_cast<TabGroupNode>(targetNode);
            else if (targetNode->getType() == DockNodeType::TAB)
                group = std::dynamic_pointer_cast<TabGroupNode>(targetNode->getParent());
            else
            {
                // targetNode — Split
                auto split = std::dynamic_pointer_cast<SplitNode>(targetNode);
                if (!split->getChildren().empty() && split->getChildren()[0]->getType() == DockNodeType::TAB_GROUP)
                    group = std::dynamic_pointer_cast<TabGroupNode>(split->getChildren()[0]);
                else
                {
                    group = std::make_shared<TabGroupNode>();
                    split->addChild(group, Percent(100));
                    manager->registerNode(group);
                }
            }

            auto tab = std::make_shared<TabNode>();
            tab->setWindow(window);
            tab->setTitle(title);
            group->addTab(tab);
            manager->registerNode(tab);

            rootNode->updateLayout(rootNode->getRect());
            return tab;
        }

        // --- Доковать как плавающее окно ---
        std::shared_ptr<FloatingNode> dockAsFloating(std::shared_ptr<WindowInterface::IWindow> window,
            const NbRect<int>& rect)
        {
            if (!window) return nullptr;

            auto floating = std::make_shared<FloatingNode>();
            floating->setWindow(window);
            floating->updateLayout(rect);
            manager->registerNode(floating);
            floatingWindows.push_back(floating);

            recalcLayout();
            return floating;
        }

        // --- Доковать относительно другого окна (split) ---
        std::shared_ptr<DockNode> dockRelative(std::shared_ptr<WindowInterface::IWindow> window,
            DockPosition position,
            std::shared_ptr<WindowInterface::IWindow> targetWindow = nullptr,
            Percent size = Percent(50))
        {
            if (!window) return nullptr;

            auto targetNode = findNodeByWindow(targetWindow);
            if (!targetNode)
                targetNode = rootNode->getChildren().empty() ? nullptr : rootNode->getChildren()[0];

            if (!targetNode) return dockAsTab(window, nullptr); // если нет ничего, просто вкладка

            if (position == DockPosition::TAB)
                return dockAsTab(window, targetWindow);

            // Если targetNode — TabNode, берем его родителя
            if (targetNode->getType() == DockNodeType::TAB)
                targetNode = targetNode->getParent();

            auto oldParent = targetNode->getParent();

            SplitNode::Orientation orientation =
                (position == DockPosition::LEFT || position == DockPosition::RIGHT)
                ? SplitNode::Orientation::HORIZONTAL
                : SplitNode::Orientation::VERTICAL;

            auto split = std::make_shared<SplitNode>(orientation);

            // Вставляем split вместо targetNode
            if (oldParent)
            {
                auto& siblings = oldParent->getChildren();
                auto it = std::find(siblings.begin(), siblings.end(), targetNode);
                if (it != siblings.end())
                    *it = split;
                split->setParent(oldParent);
            }
            else
            {
                rootNode->addChild(split);
                split->setParent(rootNode);
            }

            // Новый TabNode
            auto newTab = std::make_shared<TabNode>();
            newTab->setWindow(window);

            Percent targetPercent = Percent(100 - size.get());
            if (position == DockPosition::LEFT || position == DockPosition::TOP)
            {
                split->addChild(newTab, size);
                split->addChild(targetNode, targetPercent);
            }
            else
            {
                split->addChild(targetNode, targetPercent);
                split->addChild(newTab, size);
            }

            manager->registerNode(split);
            manager->registerNode(newTab);

            rootNode->updateLayout(rootNode->getRect());
            return newTab;
        }

        void onSize(int width, int height)
        {
            NbRect<int> rect{ 0, 0, width, height };
            rootNode->updateLayout(rect);
            recalcLayout();
        }

        std::wstring dumpTreeW() const
        {
            std::wstring result;

            std::function<void(std::shared_ptr<DockNode>, int)> visit;
            visit = [&](std::shared_ptr<DockNode> node, int level)
                {
                    if (!node) return;

                    std::wstring indent(level * 2, L' ');
                    std::wstring type;
                    switch (node->getType())
                    {
                    case DockNodeType::ROOT: type = L"ROOT"; break;
                    case DockNodeType::SPLIT: type = L"SPLIT"; break;
                    case DockNodeType::TAB_GROUP: type = L"TAB_GROUP"; break;
                    case DockNodeType::TAB: type = L"TAB"; break;
                    case DockNodeType::FLOATING: type = L"FLOATING"; break;
                    }

                    auto rect = node->getRect();
                    result += indent + type + L" [" +
                        std::to_wstring(rect.x) + L"," +
                        std::to_wstring(rect.y) + L"," +
                        std::to_wstring(rect.width) + L"," +
                        std::to_wstring(rect.height) + L"]";

                    if (node->getType() == DockNodeType::TAB)
                    {
                        auto tab = std::dynamic_pointer_cast<TabNode>(node);
                        if (tab && tab->getWindow())
                            result += L" (" + tab->getWindow()->getTitle() + L")";
                    }

                    result += L"\n";

                    for (auto& child : node->getChildren())
                        visit(child, level + 1);
                };

            visit(rootNode, 0);
            return result;
        }

    private:
        void recalcLayout()
        {
            Utils::Windows::WindowPosQueue queue;
            queue.begin(64);

            auto visit = [&](auto&& self, std::shared_ptr<DockNode> node)
                {
                    if (!node) return;

                    if (node->getType() == DockNodeType::TAB)
                    {
                        auto tab = std::dynamic_pointer_cast<TabNode>(node);
                        if (auto win = tab->getWindow())
                            queue.push(win->getHandle(), node->getRect());
                    }
                    else if (node->getType() == DockNodeType::FLOATING)
                    {
                        auto floating = std::dynamic_pointer_cast<FloatingNode>(node);
                        if (auto win = floating->getWindow())
                            queue.push(win->getHandle(), node->getRect());
                    }

                    for (auto& child : node->getChildren())
                        self(self, child);
                };

            visit(visit, rootNode);
            queue.apply();
        }

        std::shared_ptr<DockNode> findNodeByWindow(std::shared_ptr<WindowInterface::IWindow> window)
        {
            if (!window) return nullptr;
            for (auto& [id, nodeWeak] : manager->getRegistry())
            {
                auto node = nodeWeak.lock();
                if (!node) continue;

                if (node->getType() == DockNodeType::TAB)
                {
                    auto tab = std::dynamic_pointer_cast<TabNode>(node);
                    if (tab->getWindow() == window)
                        return tab;
                }
                else if (node->getType() == DockNodeType::FLOATING)
                {
                    auto floating = std::dynamic_pointer_cast<FloatingNode>(node);
                    if (floating->getWindow() == window)
                        return floating;
                }
            }
            return nullptr;
        }

    private:
        std::shared_ptr<WindowInterface::IWindow> mainWindow;
        std::shared_ptr<RootNode> rootNode;
        std::shared_ptr<DockTreeManager> manager;
        std::vector<std::shared_ptr<FloatingNode>> floatingWindows;
    };





};