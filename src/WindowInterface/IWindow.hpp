#ifndef NBUI_SRC_WINDOWINTERFACE_IWINDOW_HPP
#define NBUI_SRC_WINDOWINTERFACE_IWINDOW_HPP

#include <set>

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
    class IWindowStateChangedListener
    {
    public:
        virtual void onSizeChanged(const NbSize<int>& newSize) = 0;
    };

    class IWindow
    {
    public:
        IWindow() = default;
        virtual ~IWindow() = default;

        virtual void onSize(const NbSize<int>& newSize) = 0;
        virtual void show() = 0;

        const NbWindowHandle &getHandle() const noexcept { return handle; };

        const NbSize<int>& getSize() const noexcept { return state.size; };
        const NbSize<int>& getClientSize() const noexcept { return state.clientSize; };
        const NbColor& getColor() const noexcept { return state.color; };
        const NbColor& getFrameColor() const noexcept { return state.frameColor; };
        const FrameSize& getFrameSize() const noexcept { return state.frameSize; };
        const std::wstring& getTitle() const noexcept { return state.title; };
        const NbColor& getFontColor() const noexcept { return state.fontColor; };
        const bool isMaximized() const noexcept { return state.isMaximized; };

        const WindowStyle& getStyle() const noexcept { return style; };
        const NbRect<int> getClientRect() const noexcept { return state.clientRect; };
        void setClientRect(const NbRect<int>& rect) noexcept
        { 
            state.clientRect = rect;
            state.setSize({rect.width, rect.height});
            SetWindowPos(handle.as<HWND>(), nullptr, rect.x, rect.y, rect.width, rect.height, SWP_NOZORDER);
        };

        // state dirty flags
        bool isSizeChanged() const noexcept { return state.isSizeChanged; };

        void resetStateDirtyFlags() { state.resetDirty(); };

        CaptionButtonsContainer* getCaptionButtonsContainer() noexcept { return &captionButtonsContainer; };
        const std::vector<Widgets::IWidget*>& getWidgets() const noexcept { return widgets; };

        void linkWidget(Widgets::IWidget* widget) { widgets.push_back(widget); };
        void addStateChangedListener(IWindowStateChangedListener* listener) { stateChangedListeners.insert(listener); };
        void removeStateChangedListener(IWindowStateChangedListener* listener) { stateChangedListeners.erase(listener); };


    protected:
        NbWindowHandle                              handle;
        WindowState                                 state;
        std::set<IWindowStateChangedListener*>      stateChangedListeners;
        WindowStyle                                 style;

        IWindowRenderer*                            renderer                    = nullptr;

        CaptionButtonsContainer                     captionButtonsContainer;
        std::vector<Widgets::IWidget*>              widgets;
    
    };
};


#endif