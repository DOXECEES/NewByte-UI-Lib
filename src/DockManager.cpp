// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "DockManager.hpp"

#include <queue>
#include <stack>

#include "Utils/Windows/WindowPositionQueue.hpp"

static inline NbRect<int> sanitizeRectForWindow(const NbRect<int>& desired, const NbRect<int>& parentRect) noexcept
{
    NbRect<int> r = desired;

    // Минимальные разрешённые размеры (подбирай под проект)
    constexpr int kMinWidth = 24;
    constexpr int kMinHeight = 24;

    if (r.width < kMinWidth)  r.width = kMinWidth;
    if (r.height < kMinHeight) r.height = kMinHeight;

    // Корректируем x/y чтобы левый/верхний угол был внутри parent
    if (r.x < parentRect.x) r.x = parentRect.x;
    if (r.y < parentRect.y) r.y = parentRect.y;

    // Ограничиваем по правой/нижней границе
    const int maxW = parentRect.x + parentRect.width - r.x;
    const int maxH = parentRect.y + parentRect.height - r.y;

    if (r.width > maxW)  r.width = std::max(0, maxW);
    if (r.height > maxH) r.height = std::max(0, maxH);

    // Если после ограничений размеры уменьшились ниже минимума — сдвинем влево/вверх при возможности
    if (r.width < kMinWidth)
    {
        // постараемся сдвинуть влево так, чтобы вместиться
        int needed = kMinWidth - r.width;
        int shift = std::min(needed, r.x - parentRect.x);
        r.x -= shift;
        r.width += shift;
        if (r.width < kMinWidth) r.width = kMinWidth; // последняя защита
    }
    if (r.height < kMinHeight)
    {
        int needed = kMinHeight - r.height;
        int shift = std::min(needed, r.y - parentRect.y);
        r.y -= shift;
        r.height += shift;
        if (r.height < kMinHeight) r.height = kMinHeight;
    }

    return r;
}


DockManager::DockManager(WindowInterface::IWindow *window)
    : window(window)
    , tree(new DockTree(window->getClientRect()))
{

}


void DockManager::show()
{
    // load from file

    //



}

void DockManager::addWindow(WindowInterface::IWindow *parent, WindowInterface::IWindow *dockWindow, DockPlacement placement)
{
    std::shared_ptr<DockNode> parentNode = tree->find(parent);
    
    
    if(parentNode == nullptr)
    {
        parentNode = tree->getRoot();
    }

    std::shared_ptr<DockNode> insertNode = std::make_shared<DockWindow>(dockWindow, placement, parentNode);
    std::shared_ptr<DockNode> parentOfInsertedNode = tree->insert(parentNode, insertNode);

    update(parentOfInsertedNode, placement);

}

void DockManager::update(const std::shared_ptr<DockNode>& parent, DockPlacement placement)
{
    // parent 
    // placement
    parent->calculate(placement);
    //tree->calculateLayout(tree->getRoot());

    std::stack<std::shared_ptr<DockNode>> nodes;
    nodes.push(tree->getRoot());

    while (!nodes.empty())
    {
        std::shared_ptr<DockNode> current = nodes.top();
        nodes.pop();

        if (current->isWindow())
        {
            std::shared_ptr<DockWindow> currentWindow = std::dynamic_pointer_cast<DockWindow>(current);
            currentWindow->getWindow()->setClientRect(currentWindow->getRect());
            Debug::debug(currentWindow->getWindow()->getTitle());
        }

        auto childs = current->getChilds();
        for (const auto& i : childs)
        {
            if (i)
            {
                nodes.push(std::dynamic_pointer_cast<DockNode>(i));
            }
        }
    }
}
// добавь в DockManager.h приватное поле:
NbRect<int> lastRootRect = { 0,0,0,0 };

// ------------------------------------------------------------------
// onSize — вызывается из WM_SIZE
// ------------------------------------------------------------------
void DockManager::onSize(const NbRect<int>& newRect)
{
    auto root = tree->getRoot();
    if (!root) return;

    root->setRect(newRect);
    root->calculate(DockPlacement::CENTER); // рекалькуляция всего дерева

    Utils::Windows::WindowPosQueue queue;
    queue.begin(0);

    std::queue<std::shared_ptr<DockNode>> nodes;
    nodes.push(root);

    constexpr int kCaptionHeight = 35;

    while (!nodes.empty())
    {
        auto current = nodes.front();
        nodes.pop();

        if (current->isWindow())
        {
            auto winNode = std::dynamic_pointer_cast<DockWindow>(current);
            NbRect<int> rc = winNode->getRect();

            // Ограничиваем rect родителем
            rc = sanitizeRectForWindow(rc, newRect);

            // Конвертируем в клиентскую область
            rc.y += kCaptionHeight;
            rc.height -= kCaptionHeight;
            rc.height = std::max(0, rc.height);

            winNode->getWindow()->setClientRect(rc);
            queue.push(winNode->getWindow()->getHandle(), rc);
        }

        for (auto& c : current->getChilds())
            if (c) nodes.push(std::static_pointer_cast<DockNode>(c));
    }

    queue.apply();
}






// ------------------------------------------------------------------
// Новый метод — использует текущие (старые) rect внутри узлов как исходные.
// Это устраняет накопление ошибок при многократных ресайзах.
// ------------------------------------------------------------------
void DockManager::rescaleNodeUsingOldRects(const std::shared_ptr<DockNode>& node, const NbSize<float>& scaleFactor)
{
    if (!node) return;

    // Важное: берем origRect до изменения (node->getRect() возвращает "старый" rect,
    // потому что мы ещё его не меняли в этом onSize)
    const NbRect<int> origRect = node->getRect();

    NbRect<int> newRect;
    newRect.x = static_cast<int>(std::lround(origRect.x * scaleFactor.width));
    newRect.y = static_cast<int>(std::lround(origRect.y * scaleFactor.height));
    newRect.width = static_cast<int>(std::lround(origRect.width * scaleFactor.width));
    newRect.height = static_cast<int>(std::lround(origRect.height * scaleFactor.height));

    node->setRect(newRect);

    if (node->isContainer())
    {
        for (auto& child : node->getChilds())
        {
            if (child)
            {
                auto c = std::static_pointer_cast<DockNode>(child);
                rescaleNodeUsingOldRects(c, scaleFactor);
            }
        }
    }
}





