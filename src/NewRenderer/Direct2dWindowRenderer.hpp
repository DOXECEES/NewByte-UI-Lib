#ifndef NBUI_SRC_NEWRENDERER_DIRECT2DWINDOWRENDERER_HPP
#define NBUI_SRC_NEWRENDERER_DIRECT2DWINDOWRENDERER_HPP

#include "WindowInterface/IWindowRenderer.hpp"
#include "WindowInterface/IWindow.hpp"

#include "Direct2dRenderContext.hpp"
#include "Direct2dResourceManager.hpp"

#include <d2d1.h>
#pragma comment(lib, "d2d1")

namespace Renderer
{
    class Direct2dWindowRenderer : public WindowInterface::IWindowRenderer
    {
    public:

        Direct2dWindowRenderer() = delete;

        Direct2dWindowRenderer(WindowInterface::IWindow* window);
        ~Direct2dWindowRenderer();
        void render(WindowInterface::IWindow* window) override;
        void resize(WindowInterface::IWindow* window) override;

    private:
        Direct2DRenderContext* ctx;
        Direct2DResourceManager* resourceManager;
    };
};

#endif 
