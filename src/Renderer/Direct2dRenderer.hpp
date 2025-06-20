#ifndef SRC_RENDERER_DIRECT2DRENDERER_HPP
#define SRC_RENDERER_DIRECT2DRENDERER_HPP

#include "../WindowInterface/IWindowRenderer.hpp"
#include "../WindowInterface/IWindow.hpp"

#include "Direct2dCaptionButtonRenderer.hpp"
#include "Direct2dWidgetRenderer.hpp"

#include "Direct2dWrapper.hpp"

#include <d2d1.h>
#pragma comment(lib, "d2d1")

namespace Renderer
{
    class Direct2dRenderer : public WindowInterface::IWindowRenderer
    {
        public:

            Direct2dRenderer() = delete;

            Direct2dRenderer(WindowInterface::IWindow *window);
            ~Direct2dRenderer() = default;
            void render(WindowInterface::IWindow *window) override;
            void resize(WindowInterface::IWindow *window) override;

        private:
            Direct2dHandleRenderTarget renderTarget;


    };
};

#endif