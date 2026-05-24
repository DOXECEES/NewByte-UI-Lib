#ifndef NBUI_SRC_LAYOUTBUILDER_HPP
#define NBUI_SRC_LAYOUTBUILDER_HPP

#include <NbCore.hpp>
#include <Core.hpp>
#include <string>
#include <memory>
#include <functional>
#include <filesystem>

#include <Alghorithm.hpp>

#include "Widgets/WidgetStyle.hpp"
#include "Widgets/Indentations.hpp"
#include "Widgets/Menu.hpp"
#include "Layout/LayoutNode.hpp"
#include "Layout/LayoutWidget.hpp"
#include "Signal.hpp"

#include "Renderer/TextAlignment.hpp"

namespace Widgets 
{
    class IWidget;
    enum class TextAlign;
    enum class AssetType;
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
        CollapsibleLayout() noexcept : VLayout()
        {
        }

        void setCollapsed(bool collapsed) noexcept
        {
            if (isCollapsed == collapsed)
            {
                return;
            }
            isCollapsed = collapsed;

            if (isCollapsed)
            {
                m_savedHeightType = style.heightSizeType;
                style.heightSizeType = SizeType::AUTO;
            }
            else
            {
                style.heightSizeType = m_savedHeightType;
            }

            this->markDirty();
        }

        void toggle() noexcept
        {
            setCollapsed(!isCollapsed);
        }

        void measure(const NbSize<int>& available) noexcept override
        {
            if (children.empty())
            {
                return;
            }

            children[0]->measure(available);
            NbSize<int> headerSize = children[0]->getMeasuredSize();

            if (isCollapsed)
            {
                measuredSize = headerSize;
            }
            else
            {
                VLayout::measure(available);

            }
        }

        void layout(const NbRect<int>& bounds) noexcept override
        {
            this->layoutRect = bounds;

            if (bounds.height <= 0 || bounds.width <= 0)
            {
                for (auto& child : children)
                {
                    child->setRect({0, 0, 0, 0});
                    child->layout({0, 0, 0, 0}); 
                }
                return;
            }

            if (children.empty())
            {
                return;
            }

            if (isCollapsed)
            {
                children[0]->measure({bounds.width, bounds.height});
                NbSize<int> headerSize = children[0]->getMeasuredSize();

                NbRect<int> headerRect = {bounds.x, bounds.y, bounds.width, headerSize.height};
                children[0]->layout(headerRect);

                for (size_t i = 1; i < children.size(); ++i)
                {
                    children[i]->setRect({0, 0, 0, 0});
                    children[i]->layout({0, 0, 0, 0});
                }
            }
            else
            {
                VLayout::layout(bounds);
            }

        }

    private:
        bool     isCollapsed       = false;
        SizeType m_savedHeightType = SizeType::AUTO;
    };

} // namespace NNsLayout

namespace Widgets
{
    class ScrollArea : public IWidget
    {
    public:
        ScrollArea()
            : IWidget(
                  {0,
                   0,
                   0,
                   0}
              )
        {
        }

        void setManagedLayout(NNsLayout::VLayout* l)
        {
            m_layout = l;
        }

        const char* getClassName() const override
        {
            return "ScrollArea";
        }


        const NbSize<int>& measure(const NbSize<int>& available) noexcept override
        {
            if (m_layout)
            {
                m_layout->measure({available.width, 100000});
            }
            measuredSize = available;
            return measuredSize;
        }

        void layout(const NbRect<int>& rect) noexcept override
        {
            this->setRect(rect); 

            if (m_layout)
            {
                m_layout->layout(rect);
            }
        }


        bool hitTest(const NbPoint<int>& pos) override
        {
            return false;
        }

        void onMouseWheel(
            const NbPoint<int>& pos,
            int                 delta
        ) override
        {
            if (m_layout)
            {
                int contentHeight = m_layout->getMeasuredSize().height;
                int viewHeight    = rect.height; 
                int maxScroll     = (std::max)(0, contentHeight - viewHeight);

                int scrollAmount = (delta / 120) * 40;
                int newOffset =
                    (nbstl::clamp)(m_layout->getScrollOffset() - scrollAmount, 0, maxScroll);

                m_layout->setScrollOffset(newOffset);
                m_layout->markDirty();
            }
        }

    private:
        NNsLayout::VLayout* m_layout = nullptr;
    };
} // namespace Widgets

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

        static LayoutBuilder scrollBox()
        {
            auto scrollWidget = new Widgets::ScrollArea();

            auto vLayout = std::make_unique<NNsLayout::VLayout>();
            scrollWidget->setManagedLayout(vLayout.get());

            auto widgetNode = std::make_unique<NNsLayout::LayoutWidget>(scrollWidget);

            auto* vLayoutPtr = vLayout.get();
            widgetNode->addChild(std::move(vLayout));

            LayoutBuilder b;
            b.node = std::move(widgetNode);
            b.currentNode = vLayoutPtr;
            b.currentNode->style.widthSizeType  = NNsLayout::SizeType::RELATIVE;
            b.currentNode->style.width          = 1.0f;
            b.currentNode->style.heightSizeType = NNsLayout::SizeType::RELATIVE;
            b.currentNode->style.height         = 1.0f;

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
        
        static LayoutBuilder thumbnail(
            const std::wstring&          name,
            const std::wstring&          type,
            Widgets::AssetType           assetType,
            const std::filesystem::path& path
        );
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

        static LayoutBuilder section(
            const std::wstring& title,
            bool                collapsed = false
        );



    private:

        LayoutBuilder&& buttonGroup(NNsLayout::ButtonGroupType type) &&;


        std::unique_ptr<NNsLayout::LayoutNode>      node;
        NNsLayout::LayoutNode*                      currentNode = nullptr;
        Widgets::IWidget*                           currentNodeWidget = nullptr;

    };
}

#endif