#ifndef SRC_WINDOWINTERFACE_IWINDOWRENDERER_HPP
#define SRC_WINDOWINTERFACE_IWINDOWRENDERER_HPP

namespace WindowInterface
{
    class IWindow;  
    class IWindowRenderer
    {
    public:
        virtual void render(IWindow *window) = 0;
        virtual void resize(IWindow *window) = 0;
        virtual ~IWindowRenderer() = default;
    };
}

#endif
