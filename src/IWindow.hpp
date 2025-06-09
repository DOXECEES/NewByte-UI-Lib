#ifndef SRC_IWINDOW_HPP
#define SRC_IWINDOW_HPP

#include <Windows.h>

#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include "Renderer/Renderer.hpp"

#include <string>
#include <windowsx.h>
#include <commctrl.h>

#include <sstream>
#include <variant>

#include "Core.hpp"





class IWindow
{
public:
    static constexpr int MIN_WINDOW_WIDTH   = 400;
    static constexpr int MIN_WINDOW_HEIGHT  = 300;

    struct FrameSize
    {
        int right   = 5;
        int left    = 5;
        int top     = 30;
        int bot     = 5;
    };

    IWindow() 
    {
    }


    void setFrameSize(const FrameSize &newFrameSize) 
    {
        frameSize = newFrameSize;
    }

    inline const FrameSize &getFrameSize() const { return frameSize; }

    void setBackgroundColor(NbColor color)
    {
        SafeRelease(&frameColor);
        backgroundColor = renderer.createSolidColorBrush(pRenderTarget, color);
        // InvalidateRect(handle, NULL, TRUE);
        // UpdateWindow(handle);
    };

    void setFrameColor(NbColor color)
    {
        SafeRelease(&frameColor);
        frameColor = renderer.createSolidColorBrush(pRenderTarget, color);
    };

    void setTitle(std::wstring_view str)
    {
        title = str.data();
    }

    void redraw() 
    {
        if(this == nullptr) return;
        SetWindowPos(handle, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_DRAWFRAME | SWP_FRAMECHANGED);
        SendMessage(handle, WM_PAINT, 0, 0);
    }

    void setSize(NbSize<int> newSize)
    {
        SetWindowPos(handle, 0, 0, 0, newSize.width, newSize.height, SWP_NOMOVE | SWP_NOZORDER);
    }

    void setPosition(NbPoint<int> newPos)
    {
        SetWindowPos(handle, 0, newPos.x, newPos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }

    NbSize<int> getSize() const
    {
        return size;
    }

    NbPoint<int> getPosition() const
    {
        return position;
    }

    void onDefaultNonClientDoubleClick()
    {
        
    }

    virtual void onClientPaint()                                = 0;
    virtual void onClientMouseMove(NbPoint<int> pos)            = 0;
    virtual void onClientMouseLeave()                           = 0;
    virtual void onClientMouseClick(NbPoint<int> pos)           = 0;
    virtual void onClientMouseRelease(NbPoint<int> pos)         = 0;
    virtual void onNonClientPaint(HRGN region)                  = 0;
    virtual void onNonClientClick(NbPoint<int> pos)             = 0;
    virtual void onNonClientHover(NbPoint<int> pos)             = 0;
    virtual void onNonClientDoubleClick(NbPoint<int> pos)       = 0;
    virtual void onNonClientRelease(NbPoint<int> pos)           = 0;
    virtual void onNonClientClickAndHold(NbPoint<int> pos)      = 0;
    virtual void resetNonClientState()                          = 0; 
    virtual void onNonClientLeave()                             = 0;
    virtual void onSizeChanged(const NbSize<int>& newSize)      = 0;
    virtual bool isInExcludedNonClientArea(NbPoint<int> pos)    { return false; };

    void CheckGDILeaks() {
        static int lastCount = 0;
        int currentCount = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
        
        if(currentCount > lastCount + 10) {
            OutputDebugString((L"GDI Leak detected: " + std::to_wstring(currentCount)).c_str());
        }
        lastCount = currentCount;
    }



    bool run()
    {
        MSG msg = {};
        while (GetMessage(&msg, nullptr, 0, 0))
        {
            CheckGDILeaks();
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return (int)msg.wParam;
    }

protected:
    void initDirect2d() noexcept
    {
        pRenderTarget = renderer.createHwndRenderTarget(handle, size);
        frameColor = renderer.createSolidColorBrush(pRenderTarget, NbColor{ 32, 32, 32 });
        backgroundColor = renderer.createSolidColorBrush(pRenderTarget, NbColor{ 46, 46, 46 });
    }

protected:
    class WindowState
    {
        NbPoint<int>            position            = { };
        NbSize<int>             size                = { 1, 1 };
    };

    NbWindowHandle          handle;

    
    Renderer::Renderer      renderer;
    // TODO : убрать в рендер
    ID2D1HwndRenderTarget*  pRenderTarget       = nullptr;  

    ID2D1SolidColorBrush*   backgroundColor     = nullptr;  
    ID2D1SolidColorBrush*   frameColor          = nullptr;
    // END_TODO;

    FrameSize               frameSize           = {};
    NbPoint<int>            minWindowSize       = { MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT };

    NbPoint<int>            position            = { };
    NbPoint<int>            prevPosition        = { };
    NbSize<int>             size                = { 1, 1 };
    NbSize<int>             prevSize            = { };
    std::wstring            title;  
    bool                    m_bTracking         = false;
    bool                    clientTracking      = false;
    bool                    isLmbHolds          = false;
    bool                    isDraging           = false;
    HRGN                    frameRegion         = CreateRectRgn(0, 0, 0, 0);


public:
    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_NCCREATE:
        {
            SetTimer(hWnd, 1, 100, NULL);
            return TRUE;
        }
        case WM_TIMER:
        {
            redraw();
            return 0;
        }   
        case WM_LBUTTONDOWN:
        {
            onClientMouseClick(NbPoint<int>(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
            //SetCapture(hWnd);
            return 0;
        }
        case WM_ERASEBKGND:
        {
            return TRUE;
        }

        case WM_NCCALCSIZE:
        {
            if (static_cast<bool>(wParam))
            {
                return 0;
            }
            return DefWindowProc(hWnd, message, wParam, lParam);
            // if (wParam == TRUE)
            // {
            //     NCCALCSIZE_PARAMS* params = (NCCALCSIZE_PARAMS*)lParam;
            //     params->rgrc[0].left    += frameSize.left;  
            //     params->rgrc[0].top     += frameSize.top;    
            //     params->rgrc[0].right   -= frameSize.right;  
            //     params->rgrc[0].bottom  -= frameSize.bot; 
            //     return 0;
            // }
            break;
        }
        // case WM_NCPAINT:
        // {
        //     if(this == nullptr) return 0;
        //     //onNonClientPaint((HRGN)wParam);
        //     //return 0;
        // }
        case WM_NCACTIVATE: 
        {
            if (!wParam) 
                return TRUE; 
            break;
        }
        case WM_GETMINMAXINFO:
        {
            // из-за того что передача указателя происходит после CreateWindow this может быть nullptr
            // я понимаю что это UB, но другого решения пока не нашел
            if(this == nullptr) return 0;

            MINMAXINFO *minmax = (MINMAXINFO *)(lParam);
            RECT rect;
            SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
            minmax->ptMaxSize.x = (rect.right - rect.left);
            minmax->ptMaxSize.y = (rect.bottom - rect.top);
            minmax->ptMaxPosition.x = rect.left;
            minmax->ptMaxPosition.y = rect.top;
            minmax->ptMinTrackSize.x = minWindowSize.x;
            minmax->ptMinTrackSize.y = minWindowSize.y;
            return 0;
        }


        case WM_PAINT:
        {
            if(this == nullptr) return 0;
            onClientPaint();
            return 0;
        }
        case WM_SIZE:
        {
            if(this == nullptr) return 0;
            prevSize = size;

            size.width = LOWORD(lParam);
            size.height = HIWORD(lParam);

            onSizeChanged({size.width, size.height});

            RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
            UpdateWindow(hWnd);
            return 0;
        }

        case WM_NCMOUSEMOVE:
        {
            if (!m_bTracking)
            {
                TRACKMOUSEEVENT tme;
                tme.cbSize = sizeof(TRACKMOUSEEVENT);
                tme.dwFlags = tme.dwFlags = TME_NONCLIENT | TME_LEAVE;
                tme.hwndTrack = hWnd;
                TrackMouseEvent(&tme);

                m_bTracking = true;  
            }
           

            int xPos = GET_X_LPARAM(lParam); 
            int yPos = GET_Y_LPARAM(lParam); 

            onNonClientHover({xPos, yPos});
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            if (!clientTracking)
            {
                TRACKMOUSEEVENT tme;
                tme.cbSize = sizeof(TRACKMOUSEEVENT);
                tme.dwFlags = tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = hWnd;
                TrackMouseEvent(&tme);

                clientTracking = true;  
            }


            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
                            

            if (GetCapture() == handle)
            {
                if(isDraging)
                {
                    POINT currentpos;
                    GetCursorPos(&currentpos);
                    int dx =  currentpos.x - prevPosition.x;
                    int dy =  currentpos.y - prevPosition.y;
                
                    SetWindowPos(handle, 0, dx, dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                }

                if(isLmbHolds)
                {
                    //onNonClientClickAndHold({x, y});
                }
            }

            onClientMouseMove({x, y});

            return 0;
        }

        case WM_MOUSELEAVE:
        {
            clientTracking = false;

            onClientMouseLeave();
            return 0;
        }

        case WM_NCMOUSELEAVE:
        {
            m_bTracking = false;
            
            onNonClientLeave();   
            return 0;
        }

        case WM_NCLBUTTONDOWN:
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
            LRESULT hitTest = DefWindowProc(hWnd, WM_NCHITTEST, 0, lParam);
        
            if (hitTest == HTCAPTION
                || hitTest == HTRIGHT && isInExcludedNonClientArea({xPos,yPos}))
            {
                RECT rect;
                GetWindowRect(handle, &rect);
                prevPosition.x = xPos - rect.left;
                prevPosition.y = yPos - rect.top;

                onNonClientClick({xPos, yPos});
                
                isLmbHolds = true;

                if(!isInExcludedNonClientArea({xPos, yPos}))
                {
                    isDraging = true;
                    
                    HWND hPrevCapture = SetCapture(handle);
                    if (hPrevCapture == NULL && GetLastError() != 0) 
                    {
                        // Ошибка: возможно, окно недействительно или принадлежит другому потоку
                        DWORD err = GetLastError();
                        OutputDebugString((L"SetCapture failed: " + std::to_wstring(err)).c_str());
                }
                }
        
                return 0;
            }

            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        
        case WM_NCLBUTTONUP:
        {
            isLmbHolds = false;
            if(isDraging)
            {
            if (GetCapture() == hWnd) {
                    ReleaseCapture(); // Освобождаем, только если захват наш
                }
            };

            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
            onNonClientRelease({xPos, yPos});
            return 0;
        }
        case WM_LBUTTONUP:
        {
            if (GetCapture() == hWnd) {
                    ReleaseCapture(); // Освобождаем, только если захват наш
                }
            isLmbHolds = false;
            if(isDraging)
            {
                if (GetCapture() == hWnd) {
                    ReleaseCapture(); // Освобождаем, только если захват наш
                }

            }
            isDraging  = false;

            onClientMouseRelease(NbPoint<int>(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
            
            resetNonClientState();
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        case WM_NCLBUTTONDBLCLK:
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
            onNonClientDoubleClick({xPos, yPos});
            return 0;
        }
        case WM_NCHITTEST:
        {
            POINT point;
            point.x = GET_X_LPARAM(lParam);
            point.y = GET_Y_LPARAM(lParam);

            LRESULT hit = DefWindowProc(hWnd, message, wParam, lParam);
            // if (hit == HTCAPTION)
            // {
            //     hit = HTCAPTION;
            // } 
            // if((hit == HTRIGHT || hit == HTTOP ) && isInExcludedNonClientArea({xPos, yPos}))
            // {
            //     hit = HTCAPTION;
            // }
            // if(hit == HTTOPRIGHT)
            // {
            //     hit = HTCAPTION;
            // }
            // if(hit == HTCLIENT)
            // {
            //     LPCTSTR cursor = IDC_ARROW;
            //     //HCURSOR hCursor = LoadCursor(NULL, cursor);
            //     //SetCursor(hCursor);
            // }


            ScreenToClient(hWnd, &point);

            if (point.y < frameSize.top)
                return HTCAPTION;

            if (point.x < frameSize.left)
                return HTLEFT;

            RECT rc;
            GetClientRect(handle, &rc);
            if (point.x > rc.right - frameSize.right)
                return HTRIGHT;

            if (point.y < frameSize.bot) // same as bot 
                return HTTOP;

            if (point.y > rc.bottom - frameSize.bot)
                return HTBOTTOM;

            return HTCLIENT;

            
            return hit;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        }
        return 0;
    }

    // private:
    static LRESULT CALLBACK staticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        IWindow *pThis = (IWindow *)GetWindowLongPtr(hWnd, GWL_USERDATA);
        return pThis->WndProc(hWnd, message, wParam, lParam);
    }
};


#endif