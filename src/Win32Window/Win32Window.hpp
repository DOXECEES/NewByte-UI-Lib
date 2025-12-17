#ifndef NBUI_SRC_WIN32WINDOW_WIN32WINDOW_HPP
#define NBUI_SRC_WIN32WINDOW_WIN32WINDOW_HPP

#include <Windows.h>
#include <windowsx.h>

#include "../WindowInterface/IWindow.hpp"
#include "../Utils.hpp"


#include "Signal.hpp"

#include "Utils/Windows/WindowPositionQueue.hpp"


namespace Win32Window
{
    class Window : public WindowInterface::IWindow
    {
    public:
        constexpr static int        BUTTON_PRESSED_MASK         = 0x8000;
        constexpr static uint32_t   CARET_FLICKERING_TIME_MS    = 500;

        Window();
        ~Window();
        
        void onSize(const NbSize<int>& newSize) override;
        void show() override;
        void repaint() const noexcept override;

        const NbWindowHandle &getHandle() const noexcept { return handle; };

        static void hideCursor() noexcept;
        static void showCursor() noexcept;

        inline static Widgets::IWidget *focusedWidget = nullptr; // only one widget can have focus

    public:
        Signal<void(const NbRect<int>&)> onRectChanged;
        Signal<void(const NbRect<int>&)> onSizeEnd;

    private:

        bool registerWindowClass();

        LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        LRESULT onNonClientHitTest(HWND hWnd, LPARAM lParam);

        LRESULT onSize(HWND hWnd, LPARAM lParam);

        LRESULT onMinMaxInfo(HWND hWnd, LPARAM lParam);

        LRESULT onKeyDown(WPARAM wParam);

        LRESULT onChar(WPARAM wParam);

        LRESULT onMouseMove(LPARAM lParam);

        LRESULT onLeftButtonDown(LPARAM lParam);

        LRESULT onTimer(HWND hWnd);

        LRESULT onNonClientMouseLeave(HWND hWnd);

        LRESULT onNonClientMouseMove(HWND hWnd, LPARAM lParam);

        LRESULT onNonClientButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        LRESULT onExitSizeMove(HWND hWnd);

        LRESULT onCreate();

        LRESULT onPaint(HWND hWnd);
            
        inline static LRESULT CALLBACK staticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            Window *pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
            
            if (message == WM_NCCREATE)
            {
                auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
                pThis = static_cast<Window*>(cs->lpCreateParams);
                SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
            }

            if (pThis)
            {
                return pThis->wndProc(hWnd, message, wParam, lParam);
            }
            return DefWindowProc(hWnd, message, wParam, lParam); 
        }

        inline static bool  isMouseTrack = false;
        PTP_TIMER           timer;

    };
};

#endif