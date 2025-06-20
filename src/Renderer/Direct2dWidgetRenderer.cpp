#include "Direct2dWidgetRenderer.hpp"

namespace Renderer
{
    Direct2dWidgetRenderer::Direct2dWidgetRenderer(Direct2dHandleRenderTarget *renderTarget)
        :renderTarget(renderTarget)
    {

    }

    void Direct2dWidgetRenderer::render(IWidget *widget)
    {
        renderTarget->fillRectangle(widget->getRect(), widget->getColor());
    }
};