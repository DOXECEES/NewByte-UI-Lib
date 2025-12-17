#ifndef NBUI_SRC_WINDOWINTERFACE_IWINDOWRENDERER_HPP
#define NBUI_SRC_WINDOWINTERFACE_IWINDOWRENDERER_HPP

#include "../Core.hpp"
#include "../Widgets/IWidgetRenderer.hpp"

class CaptionButtonsContainer;

namespace WindowInterface
{
    class IWindow;  

    class ICaptionButtonRenderer
    {
    public:
        virtual ~ICaptionButtonRenderer() noexcept = default;
        virtual void render(CaptionButtonsContainer* container) = 0;
    };

    class IWindowRenderer
    {
    public:
        virtual void render(IWindow *window) = 0;
        virtual void resize(IWindow *window) = 0;
        virtual ~IWindowRenderer() = default;
    
    protected:

        ICaptionButtonRenderer*     captionButtonRenderer   = nullptr;
        Widgets::IWidgetRenderer*   widgetRenderer          = nullptr;
    
    };
}

#endif
