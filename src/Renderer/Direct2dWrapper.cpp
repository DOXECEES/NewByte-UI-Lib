#include "Direct2dRenderer.hpp"
#include "Direct2dWrapper.hpp"

D2D1_RECT_F Direct2dUtils::toD2D1Rect(const NbRect<int> &rect) noexcept
{
    return D2D1::RectF(static_cast<float>(rect.x), static_cast<float>(rect.y), static_cast<float>(rect.x + rect.width), static_cast<float>(rect.y + rect.height));
}

Direct2dHandleRenderTarget Direct2dWrapper::createRenderTarget(const NbWindowHandle &handle, const NbSize<int> &size) noexcept
{
    D2D1_SIZE_U renderTargetSize = D2D1::SizeU(size.width, size.height);
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
    D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(handle.as<HWND>(), renderTargetSize);
    
    ID2D1HwndRenderTarget *renderTarget = nullptr;
    if(factory->CreateHwndRenderTarget(props, hwndProps, &renderTarget) != S_OK)
    {
        return nullptr;
    }

    uint32_t dpi = GetDpiForWindow(handle.as<HWND>());
    renderTarget->SetDpi(dpi, dpi);
    return Direct2dHandleRenderTarget(renderTarget);
}

ID2D1SolidColorBrush *Direct2dWrapper::createSolidColorBrush(const Direct2dHandleRenderTarget &renderTarget, const NbColor &color) noexcept
{
    return renderTarget.createSolidBrush(color);
}
