#ifndef SRC_WINDOWINTERFACE_IWINDOW_HPP
#define SRC_WINDOWINTERFACE_IWINDOW_HPP

#include "../Core.hpp"

#include "WindowCore.hpp"
#include "IPlatformMessage.hpp"
#include "IWindowRenderer.hpp"

#include "WindowState.hpp"
#include "WindowStyle.hpp"

#include "../CaptionButtons.hpp"
#include "../Widgets/IWidget.hpp"

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
        const NbSize<int>& getClientSize() const noexcept { return state.clientSize; };
        const NbColor& getColor() const noexcept { return state.color; };
        const NbColor& getFrameColor() const noexcept { return state.frameColor; };
        const FrameSize& getFrameSize() const noexcept { return state.frameSize; };
        const std::wstring& getTitle() const noexcept { return state.title; };
        const NbColor& getFontColor() const noexcept { return state.fontColor; };

        const WindowStyle& getStyle() const noexcept { return style; };

        CaptionButtonsContainer* getCaptionButtonsContainer() noexcept { return &captionButtonsContainer; };
        const std::vector<Widgets::IWidget*>& getWidgets() const noexcept { return widgets; };

    protected:
        NbWindowHandle                  handle;
        WindowState                     state;
        WindowStyle style;

        IWindowRenderer*                renderer = nullptr;

        CaptionButtonsContainer         captionButtonsContainer;
        std::vector<Widgets::IWidget*>  widgets;
    
    };
};

#endif