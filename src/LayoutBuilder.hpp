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

namespace Widgets 
{
    class IWidget;
    enum class TextAlign;
}

namespace nbui
{
    class LayoutBuilder
    {
    public:
        static LayoutBuilder widget(Widgets::IWidget* w);
        static LayoutBuilder label(const std::wstring& text);
        static LayoutBuilder hBox();
        static LayoutBuilder vBox();
        static LayoutBuilder spacer();
        static LayoutBuilder treeView();

        LayoutBuilder&& child(LayoutBuilder&& childBuilder)&&;
        LayoutBuilder&& background(const NbColor& color)&&;
        LayoutBuilder&& color(const NbColor& color)&&;
        LayoutBuilder&& border(
            int width,
            Border::Style style = Border::Style::SOLID,
            const NbColor& color = {}
        )&&;
        LayoutBuilder&& margin(const Margin<int>& margin)&&;
        LayoutBuilder&& padding(const Padding<int>& padding)&&;

        LayoutBuilder&& relativeWidth(float w)&&;
        LayoutBuilder&& relativeHeight(float h)&&;
        LayoutBuilder&& absoluteWidth(int w)&&;
        LayoutBuilder&& absoluteHeight(int h)&&;

        LayoutBuilder&& text(const std::wstring& t)&&;
        LayoutBuilder&& style(std::function<void(NNsLayout::LayoutStyle&)> f)&&;

        LayoutBuilder&& checked(bool state)&&;
        LayoutBuilder&& fontSize(int size)&&;
        LayoutBuilder&& textAlign(Widgets::TextAlign align)&&;


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
            if (auto w = dynamic_cast<T*>(currentNode->getOwner()))
            {
                func(w);
            }
            return std::move(*this);
        }

        std::unique_ptr<NNsLayout::LayoutNode> build()&&;

    private:
        std::unique_ptr<NNsLayout::LayoutNode>      node;
        NNsLayout::LayoutNode*                      currentNode = nullptr;
    };
}

#endif