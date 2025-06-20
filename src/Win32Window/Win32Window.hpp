#ifndef SRC_WIN32WINDOW_WIN32WINDOW_HPP
#define SRC_WIN32WINDOW_WIN32WINDOW_HPP

#include <Windows.h>
#include <windowsx.h>

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
                case WM_NCCALCSIZE:
                {
                    return 0;
                }
                case WM_GETMINMAXINFO:
                {
                    MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);

                    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
                    MONITORINFO mi = { sizeof(mi) };
                    if (GetMonitorInfo(hMonitor, &mi))
                    {
                        RECT rcWork = mi.rcWork;       
                        RECT rcMonitor = mi.rcMonitor; 

                        mmi->ptMaxSize.x = rcWork.right - rcWork.left;
                        mmi->ptMaxSize.y = rcWork.bottom - rcWork.top;

                        mmi->ptMaxPosition.x = rcWork.left;
                        mmi->ptMaxPosition.y = rcWork.top;

                        mmi->ptMinTrackSize.x = state.minSize.width;
                        mmi->ptMinTrackSize.y = state.minSize.height;
                    }

                    return 0; 
                }
                case WM_SIZE:
                {
                    const int borderRadius = style.getBorderRadius();
                    RECT rc;
                    GetClientRect(hWnd, &rc);
                    HRGN hRgn;
                    if(IsMaximized(hWnd))
                    {
                        hRgn = CreateRoundRectRgn(0, 0, rc.right + state.frameSize.right, rc.bottom + state.frameSize.bot, 0, 0);

                    }
                    else
                    {
                        hRgn = CreateRoundRectRgn(0, 0, rc.right, rc.bottom, borderRadius, borderRadius);
                    }

                    SetWindowRgn(hWnd, hRgn, TRUE);
                    DeleteObject(hRgn);

                    onSize({LOWORD(lParam), HIWORD(lParam)});
                    return 0;
                }
                case WM_NCHITTEST:  // if's order important
                {
                    int x = GET_X_LPARAM(lParam);
                    int y = GET_Y_LPARAM(lParam);

                    POINT point = {x,y};

                    ScreenToClient(hWnd, &point);
                    
                    constexpr int SIZE_TO_MOVE_ARROW = 10;

                    if(point.x < SIZE_TO_MOVE_ARROW && point.y < SIZE_TO_MOVE_ARROW)
                        return HTTOPLEFT;

                    if (point.y < SIZE_TO_MOVE_ARROW)
                        return HTTOP;
                  
                    RECT rc;
                    GetClientRect(hWnd, &rc);
                  
                    if(point.y > rc.bottom - state.frameSize.bot && point.x < SIZE_TO_MOVE_ARROW)
                        return HTBOTTOMLEFT;

                    if(point.x > rc.right - state.frameSize.right && point.y > rc.bottom - state.frameSize.bot)
                        return HTBOTTOMRIGHT;

                    if (point.x < state.frameSize.left)
                        return HTLEFT;

                    if (point.x > rc.right - state.frameSize.right)
                        return HTRIGHT;

                    if (point.y < state.frameSize.bot) // same as bot 
                        return HTTOP;

                    if (point.y < state.frameSize.top)
                        return HTCAPTION;

                    if (point.y > rc.bottom - state.frameSize.bot)
                        return HTBOTTOM;

                    return HTCLIENT;
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