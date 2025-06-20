#ifndef SRC_WIDGETS_IWIDGETRENDERER_HPP
#define SRC_WIDGETS_IWIDGETRENDERER_HPP

#include "IWidget.hpp"

namespace Widgets
{
    class IWidgetRenderer
    {
    public:
        virtual void render(IWidget* widget) = 0;
    };
}

#endif 