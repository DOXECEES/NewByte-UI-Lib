// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#include "Direct2dCaptionButtonRenderer.hpp"


Renderer::Direct2dCaptionButtonRenderer::Direct2dCaptionButtonRenderer(Direct2dHandleRenderTarget *renderTarget)
    : renderTarget(renderTarget)
{
}

void Renderer::Direct2dCaptionButtonRenderer::render(CaptionButtonsContainer *container)
{
    auto reverseIterator = container->getButtons().rbegin();
    const NbRect<int>& paintArea = container->getPaintArea();
    for(auto i = reverseIterator; i != container->getButtons().rend(); ++i)
    {
        const NbRect<int>& localRect = i->rect;
        NbRect<int> rect = {paintArea.width - localRect.width - i->rect.x
                            , localRect.y
                            , localRect.width
                            , localRect.height
        };

        if(i->isHovered)
        {
            renderTarget->fillRectangle(rect, i->hoverColor);
        }
        else
        {
            renderTarget->fillRectangle(rect, i->color);
        }

        renderTarget->drawText(i->text, rect, NbColor(255, 255, 255));
    }
}