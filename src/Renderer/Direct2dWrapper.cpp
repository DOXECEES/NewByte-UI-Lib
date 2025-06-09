#include "Direct2dRenderer.hpp"
#include "Direct2dWrapper.hpp"

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
    
    return Direct2dHandleRenderTarget(renderTarget);
}

ID2D1SolidColorBrush *Direct2dWrapper::createSolidColorBrush(const Direct2dHandleRenderTarget &renderTarget, const NbColor &color) noexcept
{
    return renderTarget.createSolidBrush(color);
}
