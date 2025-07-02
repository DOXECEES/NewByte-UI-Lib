#include "Direct2dWidgetRenderer.hpp"

namespace Renderer
{
    Direct2dWidgetRenderer::Direct2dWidgetRenderer(Direct2dHandleRenderTarget *renderTarget)
        :renderTarget(renderTarget)
    {

    }

    void Direct2dWidgetRenderer::render(IWidget *widget)
    {

        const char *widgetName = widget->getClassName();
        if(strncmp(widgetName, "Button", sizeof(widgetName) / sizeof(char)) == 0 )
        {
            renderButton(widget);
        }

    }
    void Direct2dWidgetRenderer::renderButton(IWidget *widget)
    {
        Widgets::Button *button = dynamic_cast<Widgets::Button*>(widget);

        const NbColor &color = button->getIsHover() ? button->getHoverColor() : button->getColor();
        renderTarget->fillRectangle(button->getRect(), color);
        renderTarget->drawText(button->getText(), button->getRect(), NbColor(255, 255, 255));
    }
};