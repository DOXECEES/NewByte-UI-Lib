#ifndef SRC_WIN32WINDOW_WIN32WINDOW_HPP
#define SRC_WIN32WINDOW_WIN32WINDOW_HPP

#include <Windows.h>

#include "../WindowInterface/IWindow.hpp"


namespace Win32Window
{
    class Window : public WindowInterface::IWindow
    {
    public:
        Window();
        ~Window() {};

        virtual void onSize(const NbSize<int>& newSize) override;

    private:
        bool registerWindowClass();

        LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch(message)
            {
                case WM_PAINT:
                {
                    renderer->render(this);
                    return 0;
                }
                case WM_SIZE:
                {
                    onSize({LOWORD(lParam), HIWORD(lParam)});
                    return 0;
                }
                case WM_DESTROY:
                {
                    PostQuitMessage(0);
                    return 0;
                }
            }
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        
    inline static LRESULT CALLBACK staticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        Window *pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWL_USERDATA));
        
        if (message == WM_NCCREATE)
        {
            auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
            pThis = static_cast<Window*>(cs->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
        }

        if (pThis)
            return pThis->wndProc(hWnd, message, wParam, lParam);
        return DefWindowProc(hWnd, message, wParam, lParam);

                
    }

    };
};

#endif