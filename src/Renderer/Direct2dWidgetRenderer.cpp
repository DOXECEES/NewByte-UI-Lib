#include "Direct2dWidgetRenderer.hpp"

namespace Renderer
{
    Direct2dWidgetRenderer::Direct2dWidgetRenderer(Direct2dHandleRenderTarget *renderTarget)
        :renderTarget(renderTarget)
    {

    }

    void Direct2dWidgetRenderer::render(IWidget *widget)
    {
        const NbColor &color = widget->getIsHover() ? widget->getHoverColor() : widget->getColor();
        
        renderTarget->fillRectangle(widget->getRect(), color);
    }
};