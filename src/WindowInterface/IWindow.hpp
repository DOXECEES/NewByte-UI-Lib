#ifndef SRC_WINDOWINTERFACE_IWINDOW_HPP
#define SRC_WINDOWINTERFACE_IWINDOW_HPP

#include "../Core.hpp"

#include "WindowCore.hpp"
#include "IPlatformMessage.hpp"
#include "IWindowRenderer.hpp"

#include "WindowState.hpp"

namespace WindowInterface
{
    class IWindow
    {
    public:
        IWindow() = default;
        virtual ~IWindow() = default;

        virtual void onSize(const NbSize<int>& newSize) = 0;

        const NbWindowHandle &getHandle() const noexcept { return handle; };

        const NbSize<int>& getSize() const noexcept { return state.size; };
        const NbColor& getColor() const noexcept { return state.color; };
        const NbColor& getFrameColor() const noexcept { return state.frameColor; };
        const FrameSize& getFrameSize() const noexcept { return state.frameSize; };

    protected:
        NbWindowHandle      handle;
        WindowState         state;

        IWindowRenderer*    renderer = nullptr;
    
    };
};

#endif