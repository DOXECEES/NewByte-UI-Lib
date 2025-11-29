#ifndef NBUI_SRC_WIDGETS_IWIDGETRENDERER_HPP
#define NBUI_SRC_WIDGETS_IWIDGETRENDERER_HPP

#include "IWidget.hpp"

#include "Layout/LayoutNode.hpp"

namespace Widgets
{
    class IWidgetRenderer
    {
    public:
        virtual void render(IWidget* widget, const NNsLayout::LayoutStyle& layoutStyle) = 0;
        virtual void renderPopUp() noexcept = 0;
    };
}

#endif 