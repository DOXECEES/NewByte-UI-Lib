#ifndef SRC_RENDERER_DIRECT2DWIDGETRENDERER_HPP
#define SRC_RENDERER_DIRECT2DWIDGETRENDERER_HPP

#include "Direct2dWrapper.hpp"

#include "../Widgets/IWidgetRenderer.hpp"


namespace Renderer
{
    using namespace Widgets;
    class Direct2dWidgetRenderer : public IWidgetRenderer
    {
    public:
        Direct2dWidgetRenderer(Direct2dHandleRenderTarget* renderTarget);
        void render(IWidget* widget) override;
    
    private:

        void renderButton(IWidget* widget);
        void renderTextEdit(IWidget* widget);

        void createTextLayoutForWidget(IWidget* widget);

        Direct2dHandleRenderTarget* renderTarget = nullptr;

    };


};

#endif