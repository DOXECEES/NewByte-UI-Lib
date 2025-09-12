#ifndef NBUI_SRC_WIN32WINDOW_WIN32MODALWINDOW_HPP
#define NBUI_SRC_WIN32WINDOW_WIN32MODALWINDOW_HPP

#include <Windows.h>
#include <windowsx.h>

#include "../WindowInterface/WindowCore.hpp"

#include "../WindowInterface/IWindow.hpp"
#include "../WindowInterface/WindowMapper.hpp"

namespace Win32Window
{
    class ModalWindow : public WindowInterface::IWindow
    {
    public:
        ModalWindow(IWindow* parent);
        ~ModalWindow();

        void show() override;
        void repaint() const noexcept override;

        void onSize(const NbSize<int>& newSize) override
        {
            OutputDebugString(L"Window resized\n");
            state.setSize(newSize);

            captionButtonsContainer.setPaintArea(NbRect<int>(0, 0, state.size.width, state.size.height));
            //renderer->resize(this);
        };
    private:
        IWindow* parent = nullptr;

        LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch(message)
            {
                case WM_PAINT:
                {
                    //renderer->render(this);
                    return 0;
                }
        
               
                case WM_CLOSE:
                {
                    ShowWindow(hWnd, SW_HIDE);

                    return 0;

                }
                case WM_SHOWWINDOW:
                {
                    if (wParam == true)
                        EnableWindow(parent->getHandle().as<HWND>(), false);
                    else
                    {
                        EnableWindow(parent->getHandle().as<HWND>(), TRUE);
                        SetForegroundWindow(parent->getHandle().as<HWND>());

                    }
                    return DefWindowProc(hWnd, message, wParam, lParam);
                }

                case WM_DESTROY:
                {        
                    ShowWindow(hWnd, SW_HIDE);
                    //PostQuitMessage(0);
                    return 0;
                }
            }
            int r = GetLastError();
            // if(r != 0)
            // {
            //     return 0;
            // }
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        
        inline static LRESULT CALLBACK staticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            ModalWindow *pThis = reinterpret_cast<ModalWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
            
            if (message == WM_NCCREATE)
            {
                auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
                pThis = static_cast<ModalWindow*>(cs->lpCreateParams);
                SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
            }

            if (pThis)
                return pThis->wndProc(hWnd, message, wParam, lParam);
            return DefWindowProc(hWnd, message, wParam, lParam);
        }



    };
};

#endif