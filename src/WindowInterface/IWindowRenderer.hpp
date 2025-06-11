#ifndef SRC_WINDOWINTERFACE_IWINDOWRENDERER_HPP
#define SRC_WINDOWINTERFACE_IWINDOWRENDERER_HPP

#include "../Core.hpp"

class CaptionButtonsContainer;

namespace WindowInterface
{
    class IWindow;  

    class ICaptionButtonRenderer
    {
    public:

        virtual void render(CaptionButtonsContainer* container) = 0;
    };

    class IWindowRenderer
    {
    public:
        virtual void render(IWindow *window) = 0;
        virtual void resize(IWindow *window) = 0;
        virtual ~IWindowRenderer() = default;
    
    protected:

        ICaptionButtonRenderer* captionButtonRenderer = nullptr;
    
    };
}

#endif
