// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#include "Renderer.hpp"

namespace Renderer
{
    Renderer::~Renderer()
    {
        FactorySingleton::releaseFactory();
    }

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

     
};
