#include "Renderer.hpp"

namespace Renderer
{

    ID2D1HwndRenderTarget *Renderer::createHwndRenderTarget(HWND handle, const NbSize<int> &size) const noexcept
    {
        D2D1_SIZE_U renderTargetSize = D2D1::SizeU(size.width, size.height);
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
        D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(handle, renderTargetSize);
        
        ID2D1HwndRenderTarget *renderTarget = nullptr;
        if(pFactory->CreateHwndRenderTarget(props, hwndProps, &renderTarget) != S_OK)
        {
            return nullptr;
        }
        
        return renderTarget;
    }

    ID2D1SolidColorBrush *Renderer::createSolidColorBrush(ID2D1HwndRenderTarget *renderTarget, const NbColor &color) const noexcept
    {
        ID2D1SolidColorBrush* brush = nullptr;
        renderTarget->CreateSolidColorBrush(D2D1::ColorF(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a/255.0f), &brush);
        return brush;
    }
};
