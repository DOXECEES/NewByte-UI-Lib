#ifndef NBUI_SRC_LAYOUTBUILDER_HPP
#define NBUI_SRC_LAYOUTBUILDER_HPP

#include <NbCore.hpp>
#include <Core.hpp>
#include <string>
#include <memory>
#include <functional>

#include "Widgets/WidgetStyle.hpp"
#include "Widgets/Indentations.hpp"

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


namespace nbui
{
    class LayoutBuilder
    {
    public:
        enum class StateStyle
        {
            BASE,
            ACTIVE,
            HOVER, 
            DISABLE
        };

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

        std::unique_ptr<NNsLayout::LayoutNode> build()&&;

        std::shared_ptr<Widgets::IWidget> buildRawWidget();

    private:

        LayoutBuilder&& buttonGroup(NNsLayout::ButtonGroupType type) &&;


        std::unique_ptr<NNsLayout::LayoutNode>      node;
        NNsLayout::LayoutNode*                      currentNode = nullptr;
        Widgets::IWidget*                           currentNodeWidget = nullptr;

    };
}

#endif