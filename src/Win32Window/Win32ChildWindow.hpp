#ifndef NBUI_SRC_WIN32WINDOW_WIN32CHILDWINDOW_HPP
#define NBUI_SRC_WIN32WINDOW_WIN32CHILDWINDOW_HPP

#include "../WindowInterface/IWindow.hpp"

namespace Win32Window
{
    using namespace WindowInterface;

    class ChildWindow : public IWindow
    {
        public:
        ChildWindow(IWindow *parentWindow);

        virtual void onSize(const NbSize<int>& newSize) override { };
        virtual void show() override;

        const NbWindowHandle &getHandle() const noexcept { return handle; };


        LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch(message)
            {
                case WM_SIZE:
                {
                    state.setSize({ LOWORD(lParam), HIWORD(lParam) });
                    return 0;
                }
            }
            return DefWindowProc(hWnd, message, wParam, lParam);
        }


        inline static LRESULT CALLBACK staticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            ChildWindow *pThis = reinterpret_cast<ChildWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
            
            if (message == WM_NCCREATE)
            {
                auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
                pThis = static_cast<ChildWindow*>(cs->lpCreateParams);
                SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
            }

            if (pThis)
                return pThis->wndProc(hWnd, message, wParam, lParam);
            return DefWindowProc(hWnd, message, wParam, lParam);

                    
        }
    };
};

#endif