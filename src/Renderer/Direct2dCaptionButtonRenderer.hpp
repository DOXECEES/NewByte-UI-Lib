#ifndef NBUI_SRC_RENDERER_DIRECT2DCAPTIONBUTTONRENDERER_HPP
#define NBUI_SRC_RENDERER_DIRECT2DCAPTIONBUTTONRENDERER_HPP

#include "../WindowInterface/IWindowRenderer.hpp"
#include "Direct2dWrapper.hpp"

#include "../CaptionButtons.hpp"

namespace Renderer
{

    class Direct2dCaptionButtonRenderer : public WindowInterface::ICaptionButtonRenderer
    {
    public:

        Direct2dCaptionButtonRenderer(Direct2dHandleRenderTarget* renderTarget);
        ~Direct2dCaptionButtonRenderer() = default;

        void render(CaptionButtonsContainer* container) override;

    private: 
        Direct2dHandleRenderTarget* renderTarget = nullptr;
    };
};

#endif