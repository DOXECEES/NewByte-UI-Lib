#include "Direct2dRenderer.hpp"

namespace Renderer 
{
    Direct2dRenderer::Direct2dRenderer(WindowInterface::IWindow *window)
        : renderTarget(Direct2dWrapper::createRenderTarget(window->getHandle(), window->getSize()))
    {
        captionButtonRenderer = new Direct2dCaptionButtonRenderer(&renderTarget);
        widgetRenderer = new Direct2dWidgetRenderer(&renderTarget);
    }

    void Direct2dRenderer::render(WindowInterface::IWindow *window) 
    {
        const NbSize<int>& windowSize = window->getSize(); 
        const WindowInterface::WindowStyle& style = window->getStyle();
        renderTarget.beginDraw();

        renderTarget.clear(window->getColor());
        if(window->isMaximized())
        {
            renderTarget.drawRoundedRectangle({0, 0, windowSize.width, windowSize.height}, 0 , window->getColor());
        }
        else
        {
            renderTarget.drawRoundedRectangle({0, 0, windowSize.width, windowSize.height}, style.getBorderRadius() , window->getColor());
        }



        const NbColor &frameColor = window->getFrameColor();
        const WindowInterface::FrameSize& frameSize = window->getFrameSize();
        
        NbRect<int> topBorder = { 0, 0, windowSize.width, frameSize.top};
        renderTarget.fillRectangle(topBorder, frameColor);

        NbRect<int> botBorder = {0, windowSize.height - frameSize.bot, windowSize.width, frameSize.bot};
        renderTarget.fillRectangle(botBorder, frameColor);

        NbRect<int> leftBorder = {0, frameSize.top, frameSize.left, windowSize.height - frameSize.top - frameSize.bot};
        renderTarget.fillRectangle(leftBorder, frameColor);

        NbRect<int> rightBorder = {windowSize.width - frameSize.right, frameSize.top, frameSize.right, windowSize.height - frameSize.top - frameSize.bot};
        renderTarget.fillRectangle(rightBorder, frameColor);

        renderTarget.drawText(window->getTitle(), {0, 0, windowSize.width, frameSize.top}, window->getFontColor());

        captionButtonRenderer->render(window->getCaptionButtonsContainer());

        for(auto& widget : window->getWidgets())
            widgetRenderer->render(widget);

        HRESULT hr = renderTarget.endDraw();

        if (FAILED(hr)) {
            OutputDebugString(L"EndDraw failed\n");
        }



    }
    void Direct2dRenderer::resize(WindowInterface::IWindow *window)
    {
        renderTarget.resize(window->getSize());
    }
}

