#ifndef NBUI_SRC_LAYOUTBUILDER_HPP
#define NBUI_SRC_LAYOUTBUILDER_HPP

#include <NbCore.hpp>
#include <Core.hpp>
#include <string>
#include <memory>
#include <functional>

#include "Widgets/WidgetStyle.hpp"
#include "Widgets/Indentations.hpp"
#include "Widgets/Menu.hpp"
#include "Layout/LayoutNode.hpp"
#include "Signal.hpp"

#include "Renderer/TextAlignment.hpp"

namespace Widgets 
{
    class IWidget;
    enum class TextAlign;
}

namespace nb
{
    class Color;
}

namespace NNsLayout
{
    enum class ButtonGroupType;
}


namespace NNsLayout
{
    class CollapsibleLayout : public VLayout
    {
    public:
        CollapsibleLayout() noexcept
            : VLayout(),
              isCollapsed(false)
        {
        }

        void setCollapsed(bool collapsed) noexcept
        {
            if (isCollapsed != collapsed)
            {
                isCollapsed = collapsed;
                markDirty();

                // Если у вас есть указатель на родителя, помечаем всю ветку как грязную
                LayoutNode* curr = this->getParent();
                while (curr)
                {
                    curr->markDirty();
                    curr = curr->getParent();
                }
            }
        }

        bool getCollapsed() const noexcept
        {
            return isCollapsed;
        }

        void measure(const NbSize<int>& available) noexcept override
        {
            if (!isCollapsed)
            {
                // Если развернуто, считаем как обычный VLayout
                VLayout::measure(available);
            }
            else
            {
                // Если свернуто, измеряем ТОЛЬКО первый элемент (Заголовок)
                if (!children.empty() && children[0] != nullptr)
                {
                    children[0]->measure(available);
                    measuredSize = children[0]->getMeasuredSize();

                    // Учитываем паддинги самого CollapsibleLayout (если они есть)
                    measuredSize.width += style.padding.left + style.padding.right;
                    measuredSize.height += style.padding.top + style.padding.bottom;
                }
                else
                {
                    measuredSize = {0, 0};
                }
            }
        }

        void layout(const NbRect<int>& bounds) noexcept override
        {
            if (!isCollapsed)
            {
                // Если развернуто, позиционируем как обычный VLayout
                VLayout::layout(bounds);
            }
            else
            {
                layoutRect = bounds;

                if (!children.empty() && children[0] != nullptr)
                {
                    // Позиционируем только Заголовок
                    NbRect<int> headerBounds = bounds;
                    headerBounds.x += style.padding.left;
                    headerBounds.y += style.padding.top;
                    headerBounds.width -= (style.padding.left + style.padding.right);
                    headerBounds.height = children[0]->getMeasuredSize().height;

                    children[0]->layout(headerBounds);

                    // ОСТАЛЬНЫМ элементам принудительно ставим нулевой размер,
                    // чтобы они исчезли с экрана и не перехватывали клики мышкой
                    for (size_t i = 1; i < children.size(); ++i)
                    {
                        if (children[i])
                        {
                            children[i]->setRect({0, 0, 0, 0});
                            children[i]->layout({0, 0, 0, 0}); // Опционально пробрасываем нули ниже
                        }
                    }
                }
            }
        }

    private:
        bool isCollapsed;
    };
} // namespace NNsLayout

namespace nbui
{
    class LayoutBuilder
    {
    public:

        LayoutBuilder() = default;

        explicit LayoutBuilder(NNsLayout::LayoutNode* nodePtr)
            : node(nullptr),
              currentNode(nodePtr),
              currentNodeWidget(nullptr)
        {
        }

        enum class StateStyle
        {
            BASE,
            ACTIVE,
            HOVER, 
            DISABLE
        };

        static LayoutBuilder collapsibleBox()
        {
            LayoutBuilder b;
            b.node = std::make_unique<NNsLayout::CollapsibleLayout>();
            b.currentNode = b.node.get();
            return b;
        }

        static LayoutBuilder widget(Widgets::IWidget* w);
        static LayoutBuilder label(const std::wstring& text);
        static LayoutBuilder hBox();
        static LayoutBuilder vBox();
        static LayoutBuilder grid(int columns);
        static LayoutBuilder flow();

        static LayoutBuilder spacer();
        static LayoutBuilder spacerAbsolute(
            float absolutWidth,
            float absolutHeigth
        );
        static LayoutBuilder toolbar();
        LayoutBuilder&& buttonGroupOnlyOne() &&;
        LayoutBuilder&& buttonGroupMultiple() &&;

        LayoutBuilder&& endGroup() &&;
        
        static LayoutBuilder thumbnail(const std::wstring& name, const std::wstring& type);
        static LayoutBuilder treeView();

        LayoutBuilder&& child(LayoutBuilder&& childBuilder)&&;
        

        LayoutBuilder&& background(
            const NbColor& color,
            StateStyle stateStyle = StateStyle::BASE
        ) &&;
        LayoutBuilder&& color(const NbColor& color)&&;
        LayoutBuilder&& border(
            int width,
            Border::Style style = Border::Style::SOLID,
            const NbColor& color = {},
            Border::Side side = Border::Side::ALL 
        )&&;
        LayoutBuilder&& margin(const Margin<int>& margin)&&;
        LayoutBuilder&& padding(const Padding<int>& padding)&&;

        LayoutBuilder&& relativeWidth(float w)&&;
        LayoutBuilder&& relativeHeight(float h)&&;
        LayoutBuilder&& absoluteWidth(int w)&&;
        LayoutBuilder&& absoluteHeight(int h)&&;
        LayoutBuilder&& autoWidth() &&;
        LayoutBuilder&& autoHeight() &&;

        LayoutBuilder&& spacing(int spacing)&&;

        LayoutBuilder&& text(const std::wstring& t)&&;
        LayoutBuilder&& style(std::function<void(NNsLayout::LayoutStyle&)> f)&&;

        LayoutBuilder&& checked(bool state)&&;
        LayoutBuilder&& checkedGroupIndex(bool state, int index)&&;

        LayoutBuilder&& fontSize(int size)&&;
        LayoutBuilder&& textAlignment(TextFormatAlignment align)&&;


        template<
            typename Publisher,
            typename... Args,
            typename Func
        >
        LayoutBuilder&& onEvent(Signal<void(Args...)> Publisher::* signal, Func&& func)
        {
            if (auto w = dynamic_cast<Publisher*>(currentNode->getOwner()))
            {
                subscribe(w, signal, std::forward<Func>(func));
            }
            return std::move(*this);
        }

        template<typename T, typename Func>
        LayoutBuilder&& apply(Func&& func)&&
        {
            if (!currentNode)
            {
                return std::move(*this);
            }

            if (auto n = dynamic_cast<T*>(currentNode))
            {
                func(n);
            }
            else if (auto w = dynamic_cast<T*>(currentNode->getOwner()))
            {
                func(w);
            }

            return std::move(*this);

        }

        template <typename Func>
        LayoutBuilder&& menu(Func&& fn) &&
        {
            if (currentNode && currentNode->getOwner())
            {
                Widgets::Menu* menu = new Widgets::Menu();

                fn(*menu); 

                currentNode->getOwner()->addMenu(menu);
            }

            return std::move(*this);
        }

        std::unique_ptr<NNsLayout::LayoutNode> build()&&;

        std::shared_ptr<Widgets::IWidget> buildRawWidget();

        const std::unique_ptr<NNsLayout::LayoutNode>& getNode()
        {
            return node;
        }

    private:

        LayoutBuilder&& buttonGroup(NNsLayout::ButtonGroupType type) &&;


        std::unique_ptr<NNsLayout::LayoutNode>      node;
        NNsLayout::LayoutNode*                      currentNode = nullptr;
        Widgets::IWidget*                           currentNodeWidget = nullptr;

    };
}

#endif