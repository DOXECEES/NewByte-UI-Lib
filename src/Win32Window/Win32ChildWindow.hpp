#ifndef NBUI_SRC_WIN32WINDOW_WIN32CHILDWINDOW_HPP
#define NBUI_SRC_WIN32WINDOW_WIN32CHILDWINDOW_HPP

#include "../WindowInterface/IWindow.hpp"

#include "../DockManager.hpp"

#include <windowsx.h>
#include <algorithm>

namespace Win32Window
{
    using namespace WindowInterface;
    
    class ChildWindow : public IWindow
    {
    public:
        ChildWindow(IWindow *parentWindow);
        ~ChildWindow();

        void onSize(const NbSize<int>& newSize) override { };
        void show() override;
        void repaint() const noexcept override;

        const NbWindowHandle &getHandle() const noexcept { return handle; };

        void addCaption() noexcept;
        void setRenderable(bool flag) noexcept;

        inline static Widgets::IWidget* focusedWidget = nullptr; // only one widget can have focus

        LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            static Splitter* activeSplitter = nullptr;

            static NbPoint<int> dragOffset = {};
            static bool dragging = false;
            static bool clicked = true;
            switch(message)
            {
                case WM_PAINT:
                {
                    PAINTSTRUCT ps;

                    if (!isRenderable)
                    {
                        HDC hdc = BeginPaint(hWnd, &ps);
                        EndPaint(hWnd, &ps);
                        return 0;
                    }

                    HDC hdc = BeginPaint(hWnd, &ps);
                    renderer->render(this);
                    EndPaint(hWnd, &ps);

                    return 0;
                } 
                case WM_SIZE:
                {
                    state.setSize({ LOWORD(lParam), HIWORD(lParam) });
                    

                    if (renderer)
                    {
                        renderer->resize(this);
                    }

                    for (auto& listener : stateChangedListeners)
                    {
                        listener->onSizeChanged(state.clientSize);
                    }

                    InvalidateRect(hWnd, NULL, FALSE);

                    return 0;
                }
                case WM_NCHITTEST:  // if's order important
                {
                    if(state.frameSize.isEmpty())
                        return HTCLIENT;

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

                    LoadCursor(nullptr, IDC_ARROW);
                    return HTCLIENT;
                }
                case WM_LBUTTONDOWN:
                {
                    SetCapture(hWnd);

                    NbPoint<int> point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                    POINT p = { point.x, point.y };
                    MapWindowPoints(hWnd, GetParent(hWnd), &p, 1);
                    NbPoint<int> pp = Utils::toNbPoint<int>(p);

                    std::sort(widgets.begin(), widgets.end(), [](Widgets::IWidget* widget1, Widgets::IWidget* widget2) -> bool {
                        return widget1->getZIndex() > widget2->getZIndex();
                    });

                    for (const auto& widget : widgets)
                    {
                        if (!widget->isHide() && widget->hitTest(point))
                        {
                            if (focusedWidget) focusedWidget->setUnfocused();

                            focusedWidget = widget;
                            focusedWidget->setFocused();
                            clicked = true;
                            widget->onClick();
                            break;
                        }
                    }

                    for (const auto& widget : widgets)
                    {
                        
                        if (!widget->isHide() && widget->hitTestClick(point))
                        {
                            break;
                        }
                    }

                    // for (auto& i : DockManager::splitterList)
                    // {
                    //     if (i->hitTest(pp))
                    //     {
                    //         activeSplitter = i;

                    //         // ��������� ����� �������
                    //         dragOffset.x = pp.x - i->rect.x;
                    //         dragOffset.y = pp.y - i->rect.y;
                    //         dragging = true;
                    //         break;
                    //     }
                    // }
                    return 0;
                }

                case WM_MOUSEMOVE:
                {
                    NbPoint<int> point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

                    if (activeSplitter && dragging)
                    {
                        POINT p = { point.x, point.y };
                        MapWindowPoints(hWnd, GetParent(hWnd), &p, 1);
                        NbPoint<int> pp = Utils::toNbPoint<int>(p);

                        // ������������ �������� ��������� ������ �� ������� � ������������ �������
                        int shiftX = pp.x - dragOffset.x - activeSplitter->rect.x;
                        int shiftY = pp.y - dragOffset.y - activeSplitter->rect.y;

                        activeSplitter->onMove({ shiftX, shiftY });
                    }
                    else
                    {
                        bool isHoveredSet = false;
                        for (const auto& widget : widgets)
                        {
                            if(widget->isDisable())
                                continue;

                            if (!isHoveredSet && widget->hitTest(point))
                            {
                                widget->setHover();
                                isHoveredSet = true;
                            }
                            else
                            {
                                widget->setDefault();
                            }
                        }
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    return 0;
                }
                case WM_KEYDOWN:
                {
                    if (!focusedWidget)
                        return 0;

                    if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
                    {
                        focusedWidget->onButtonClicked(wParam, SpecialKeyCode::CTRL);
                    }
                    else
                    {
                        focusedWidget->onButtonClicked(wParam);
                    }

                    return 0;
                }
                case WM_LBUTTONUP:
                {


                    ReleaseCapture();
                    dragging = false;
                    activeSplitter = nullptr;
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
    private:
        bool isRenderable = true;

    };
};

#endif