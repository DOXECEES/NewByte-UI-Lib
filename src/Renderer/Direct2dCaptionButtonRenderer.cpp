#include "Direct2dCaptionButtonRenderer.hpp"


Renderer::Direct2dCaptionButtonRenderer::Direct2dCaptionButtonRenderer(Direct2dHandleRenderTarget *renderTarget)
    : renderTarget(renderTarget)
{
}

void Renderer::Direct2dCaptionButtonRenderer::render(CaptionButtonsContainer *container)
{
    auto reverseIterator = container->getButtons().rbegin();
    const NbRect<int>& paintArea = container->getPaintArea();
    for(auto i = reverseIterator; i != container->getButtons().rend(); i++)
    {
        NbRect<int> rect = {paintArea.width - i->rect.width - i->rect.x, i->rect.y, i->rect.width, i->rect.height};

        renderTarget->fillRectangle(rect, i->color);
        renderTarget->drawText(i->text, rect, NbColor(255, 255, 255));
    }
}