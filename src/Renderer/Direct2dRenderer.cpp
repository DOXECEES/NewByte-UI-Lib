// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "Layout/LayoutNode.hpp"
#include "Direct2dRenderer.hpp"

namespace Renderer 
{
    Direct2dRenderer::Direct2dRenderer(WindowInterface::IWindow *window)
        : renderTarget(Direct2dWrapper::createRenderTarget(window->getHandle(), window->getSize()))
    {
        captionButtonRenderer = new Direct2dCaptionButtonRenderer(&renderTarget);
        widgetRenderer = new Direct2dWidgetRenderer(&renderTarget);

    }

	Direct2dRenderer::~Direct2dRenderer()
	{
        delete captionButtonRenderer;
        delete widgetRenderer;
	}


	void Direct2dRenderer::render(WindowInterface::IWindow* window)
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

        const int radius = window->isMaximized() ? 0 : style.getBorderRadius();
        
        // frame lines without rounded corners //
        NbRect<int> topBorder = { 0, 0, windowSize.width, frameSize.top};
        renderTarget.fillRectangle(topBorder, frameColor);

        NbRect<int> botBorder = {0 + radius, windowSize.height - frameSize.bot, windowSize.width - radius - radius, frameSize.bot};
        renderTarget.fillRectangle(botBorder, frameColor);

        NbRect<int> leftBorder = {0, frameSize.top, frameSize.left, windowSize.height - frameSize.top - frameSize.bot - radius };
        renderTarget.fillRectangle(leftBorder, frameColor);

        NbRect<int> rightBorder = {windowSize.width - frameSize.right, frameSize.top, frameSize.right, windowSize.height - frameSize.top - frameSize.bot - radius  };
        renderTarget.fillRectangle(rightBorder, frameColor);
        //

        //NbPoint<int> start = {windowSize.width, windowSize.height - radius};
        //NbPoint<int> end = {windowSize.width - radius, windowSize.height};
        //renderTarget.drawArc(start, end, radius, frameColor);
        

        //NbRect<int> botRightCorner = {windowSize.width - radius, windowSize.height - radius, radius, radius};
        //renderTarget.fillRectangle(botRightCorner, frameColor);
        //
        if(!frameSize.isEmpty())
            renderTarget.drawText(window->getTitle(), {0, 0, windowSize.width, frameSize.top}, window->getFontColor());

        renderTarget.drawText(window->getTitle(), {0, 0, windowSize.width, windowSize.height}, window->getFontColor());
        captionButtonRenderer->render(window->getCaptionButtonsContainer());

        std::vector<const NNsLayout::LayoutNode*> stack;
        stack.push_back(window->getLayoutRoot());

        while (!stack.empty()) 
        {
            const NNsLayout::LayoutNode* node = stack.back();
            stack.pop_back();

            if (auto widgetLayout = dynamic_cast<const NNsLayout::LayoutWidget*>(node))
            {
                auto widget = widgetLayout->getWidget().get();
                if (widget)
                {
                    widgetRenderer->render(widget, widgetLayout->style);
                }
            }

            for (int i = (int)node->getChildrenSize() - 1; i >= 0; i--)
            {
                stack.push_back(node->getChildrenAt(i));
            }
        }


     

        widgetRenderer->renderPopUp();

        HRESULT hr = renderTarget.endDraw();

        if (FAILED(hr))
        {
            OutputDebugString(L"EndDraw failed\n");
        }
    }

    void Direct2dRenderer::resize(WindowInterface::IWindow *window)
    {
        renderTarget.resize(window->getSize());
    }
}

