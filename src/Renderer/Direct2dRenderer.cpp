#include "Direct2dRenderer.hpp"

namespace Renderer 
{
    Direct2dRenderer::Direct2dRenderer(WindowInterface::IWindow *window)
        : renderTarget(Direct2dWrapper::createRenderTarget(window->getHandle(), window->getSize()))
    {

    }

    void Direct2dRenderer::render(WindowInterface::IWindow *window) 
    {
        renderTarget.getRawRenderTarget()->BeginDraw();
        ID2D1SolidColorBrush* brush = renderTarget.createSolidBrush(window->getColor());
        renderTarget.getRawRenderTarget()->FillRectangle(D2D1::RectF(0, 0, window->getSize().width, window->getSize().height), brush);
        renderTarget.getRawRenderTarget()->EndDraw();
    }
    void Direct2dRenderer::resize(WindowInterface::IWindow *window)
    {
        renderTarget.resize(window->getSize());
    }
}