#include "Win32Window.hpp"
#include "Win32Global.hpp"

#include "../WindowInterface/WindowMapper.hpp"
#include "../Renderer/Direct2dRenderer.hpp"

#include "../Widgets/Button.hpp"
#include "NewRenderer/Direct2dWindowRenderer.hpp"

namespace Win32Window
{
    static NbPoint<int> getMousePoint(LPARAM lParam) noexcept
    {
        return {
            GET_X_LPARAM(lParam),
            GET_Y_LPARAM(lParam)
        };
    }

    static NbSize<int> getSizeFromLparam(LPARAM lParam) noexcept
    {
        return {
            LOWORD(lParam),
            HIWORD(lParam)
        };
    }

    Window::Window()
    {
        if(!registerWindowClass())
        {
            throw std::runtime_error("Failed to register window class.");
        }

        state.setSize({800, 600});


        HWND _handle = CreateWindowEx(0,L"NbWindowClass", L"NbWindow", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, state.size.width, state.size.height, nullptr, nullptr, nullptr, this);
        handle = NbWindowHandle::fromWinHandle(_handle);
        WindowInterface::WindowMapper::registerWindow(handle, this);
       
        LONG style = GetWindowLong(handle.as<HWND>(), GWL_STYLE);
        style &= ~(WS_CAPTION);
        style &= ~(CS_HREDRAW | CS_VREDRAW);
        SetWindowLong(handle.as<HWND>(), GWL_STYLE, style);
        //SetWindowLongPtr(handle.as<HWND>(), GWL_STYLE, GetWindowLongPtr(handle.as<HWND>(), GWL_STYLE) | WS_CLIPCHILDREN);


        SetWindowPos(handle.as<HWND>(), nullptr, 0, 0, 0, 0,
                 SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);


        // renderer
        renderer = new Renderer::Direct2dRenderer(this);


        std::vector<CaptionButton> captionButtons =
        {
            { L"✕", 50, 35, { 45, 45, 45 }, {240, 7, 23} },
            { L"🗖", 30, 35 },
            { L"🗕", 30, 35 },
        };

       captionButtons[0].setFunc([this]()
        {
            DestroyWindow(handle.as<HWND>()); 
        });
        captionButtons[1].setFunc([this]()
        {  
            if (IsZoomed(handle.as<HWND>()))
            {
                ShowWindow(handle.as<HWND>(), SW_RESTORE);
                EnumChildWindows(handle.as<HWND>(), [](HWND child, LPARAM) -> BOOL
                    {
                        PostMessage(child, WM_SETNONRENDERABLE, 0, 0); // кастомное сообщение
                        return TRUE;
                    }
                , 0);
            }
            else
            {
                ShowWindow(handle.as<HWND>(), SW_MAXIMIZE);
                EnumChildWindows(handle.as<HWND>(), [](HWND child, LPARAM) -> BOOL
                    {
                        PostMessage(child, WM_SETNONRENDERABLE, 0, 0); // кастомное сообщение
                        return TRUE;
                    }
                ,0);
            }
        });
        captionButtons[2].setFunc([this]()
        {
            CloseWindow(handle.as<HWND>()); 
        });

        //widgets.push_back(new Widgets::Button(NbRect<int>(100, 100, 100, 100)));
        
        captionButtonsContainer.addButton(captionButtons[0]);
        captionButtonsContainer.addButton(captionButtons[1]);
        captionButtonsContainer.addButton(captionButtons[2]);

    }

	Window::~Window()
	{
        CloseThreadpoolTimer(timer);
		delete renderer;
	}

	void Window::onSize(const NbSize<int>& newSize)
    {
        state.setSize(newSize);

        captionButtonsContainer.setPaintArea(NbRect<int>(0, 0, state.size.width, state.size.height));
    }

    void Window::show()
    {
        ShowWindow(handle.as<HWND>(), TRUE);
        UpdateWindow(handle.as<HWND>());
    }

	void Window::repaint() const noexcept
	{
		InvalidateRect(handle.as<HWND>(), nullptr, FALSE);
		UpdateWindow(handle.as<HWND>());
	}

	void Window::hideCursor() noexcept
    {
        while (ShowCursor(false) > 0);
    }

    void Window::showCursor() noexcept
    {
        while (ShowCursor(true) < 0);
    }

    bool Window::registerWindowClass()
    {
        WNDCLASSEX wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_OWNDC;
        wcex.lpfnWndProc = staticWndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = nullptr;
        wcex.hIcon = nullptr;
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = nullptr;
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = L"NbWindowClass";
        wcex.hIconSm = nullptr;

        return RegisterClassEx(&wcex);
    }

    LRESULT Window::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_CREATE:
        {
            return onCreate();
        }
        case WM_PAINT:
        {
            return onPaint(hWnd);
        }
        case WM_NCCALCSIZE:
        {
            return FALSE;
        }
        case WM_EXITSIZEMOVE:
        {
            return onExitSizeMove(hWnd);
        }
        case WM_NCLBUTTONDOWN:
        {
            return onNonClientButtonDown(hWnd, message, wParam, lParam);
        }
        case WM_NCMOUSEMOVE:
        {
            return onNonClientMouseMove(hWnd, lParam);
        }
        case WM_NCMOUSELEAVE:
        {
            return onNonClientMouseLeave(hWnd);
        }
        case WM_TIMER:
        {
            return onTimer(hWnd);
        }
        case WM_LBUTTONDOWN:
        {
            return onLeftButtonDown(lParam);
        }
        case WM_MOUSEMOVE:
        {
            return onMouseMove(lParam);
        }
        case WM_CHAR:
        {
            return onChar(wParam);
        }
        case WM_KEYDOWN:
        {
            return onKeyDown(wParam);
        }
        case WM_GETMINMAXINFO:
        {
            return onMinMaxInfo(hWnd, lParam);
        }
        case WM_ERASEBKGND:
        {
            return TRUE;
        }
        case WM_SIZE:
        {
            return onSize(hWnd, lParam);
        }
        case WM_NCHITTEST:  // if's order important
        {
            return onNonClientHitTest(hWnd, lParam);
        }
        case WM_NCACTIVATE:
        {
            return TRUE; // remove white frame around window
        }
        case WM_SETCURSOR:
        {
            static HCURSOR defaultCursor = LoadCursor(NULL, IDC_ARROW);
            if (LOWORD(lParam) == HTCLIENT)
            {
                SetCursor(defaultCursor);
                return TRUE;
            }
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return FALSE;
        }
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    LRESULT Window::onNonClientHitTest(HWND hWnd, LPARAM lParam)
    {
        NbPoint<int> nbPoint = getMousePoint(lParam);

        POINT point = { nbPoint.x, nbPoint.y };

        ScreenToClient(hWnd, &point);

        constexpr int SIZE_TO_MOVE_ARROW = 10;

        if (point.x < SIZE_TO_MOVE_ARROW && point.y < SIZE_TO_MOVE_ARROW)
            return HTTOPLEFT;

        if (point.y < SIZE_TO_MOVE_ARROW)
            return HTTOP;

        RECT rc;
        GetClientRect(hWnd, &rc);

        if (point.y > rc.bottom - state.frameSize.bot && point.x < SIZE_TO_MOVE_ARROW)
            return HTBOTTOMLEFT;

        if (point.x > rc.right - state.frameSize.right && point.y > rc.bottom - state.frameSize.bot)
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

    LRESULT Window::onSize(HWND hWnd, LPARAM lParam)
    {
        Debug::debug("Resize");
        const int borderRadius = style.getBorderRadius();
        RECT rect;
        GetClientRect(hWnd, &rect);

        onSize(getSizeFromLparam(lParam));

        state.clientRect = NbRect<int>(
            rect.left + state.frameSize.left
            , rect.top + state.frameSize.top
            , rect.right - rect.left - state.frameSize.left - state.frameSize.right
            , rect.bottom - rect.top - state.frameSize.top - state.frameSize.bot
        );

        renderer->resize(this);

        onRectChanged.emit(state.clientRect);

        renderer->render(this);

        ValidateRect(hWnd, NULL);
        UpdateWindow(hWnd);

        return FALSE;
    }

    LRESULT Window::onMinMaxInfo(HWND hWnd, LPARAM lParam)
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

        return FALSE;
    }

    LRESULT Window::onKeyDown(WPARAM wParam)
    {
        if (!focusedWidget)
        {
            return FALSE;
        }

        if (GetAsyncKeyState(VK_CONTROL) & BUTTON_PRESSED_MASK)
        {
            focusedWidget->onButtonClicked(wParam, SpecialKeyCode::CTRL);
        }
        else
        {
            focusedWidget->onButtonClicked(wParam);
        }

        return FALSE;
    }

    LRESULT Window::onChar(WPARAM wParam)
    {
        if (!focusedWidget)
        {
            return FALSE;
        }

        focusedWidget->onSymbolButtonClicked(static_cast<wchar_t>(wParam));

        return FALSE;
    }

    LRESULT Window::onMouseMove(LPARAM lParam)
    {
        static NbPoint<int> prevPoint = {};

        NbPoint<int> point = getMousePoint(lParam);

        for (const auto& widget : widgets)
        {
            if (widget->hitTest(point))
            {
                widget->setHover();
            }
            else
            {
                widget->setDefault();
            }
        }

        prevPoint = point;

        return FALSE;
    }

    LRESULT Window::onLeftButtonDown(LPARAM lParam)
    {
        NbPoint<int> point = getMousePoint(lParam);

        for (const auto& widget : widgets)
        {
            if (widget->hitTest(point))
            {
                if (focusedWidget)
                {
                    focusedWidget->setUnfocused();
                }
                focusedWidget = widget;
                focusedWidget->setFocused();
                widget->onClick();
            }
            widget->hitTestClick(point);
        }

        return FALSE;
    }

    LRESULT Window::onTimer(HWND hWnd)
    {
        if (focusedWidget != nullptr)
        {
            focusedWidget->onTimer();
            //InvalidateRect(hWnd, NULL, FALSE);
        }

        return FALSE;
    }

    LRESULT Window::onNonClientMouseLeave(HWND hWnd)
    {
        isMouseTrack = false;
        captionButtonsContainer.resetState();
        InvalidateRect(hWnd, NULL, FALSE);
        return FALSE;
    }

    LRESULT Window::onNonClientMouseMove(HWND hWnd, LPARAM lParam)
    {
        if (!isMouseTrack)
        {
            TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
            tme.dwFlags = TME_LEAVE | TME_NONCLIENT;
            tme.hwndTrack = hWnd;

            if (TrackMouseEvent(&tme))
            {
                isMouseTrack = true;
            }
        }
        bool needInvalidate = false;

        NbPoint<int> point = getMousePoint(lParam);
        Utils::convertToWindowSpace(hWnd, point);
        for (auto& captionButton : captionButtonsContainer)
        {
            bool prev = captionButton.getIsHovered();
            captionButton.hitTest(captionButtonsContainer.getPaintArea(), point);
            if (captionButton.getIsHovered() != prev)
            {
                needInvalidate = true;
            }
        }

        if (needInvalidate)
        {
            InvalidateRect(hWnd, NULL, FALSE);
        }

        return FALSE;
    }

    LRESULT Window::onNonClientButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        NbPoint<int> point = getMousePoint(lParam);
        Utils::convertToWindowSpace(hWnd, point);
        for (auto& captionButton : captionButtonsContainer)
        {
            captionButton.onClick(captionButtonsContainer.getPaintArea(), point);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    LRESULT Window::onExitSizeMove(HWND hWnd)
    {
        EnumChildWindows(hWnd, [](HWND child, LPARAM) -> BOOL
        {
            PostMessage(child, WM_SETNONRENDERABLE, 0, 0); 
            return TRUE;
        }, 0);

        onSizeEnd.emit(state.clientRect);
        return FALSE;
    }

    LRESULT Window::onCreate()
    {
        // SetTimer - piece of shit
        timer = CreateThreadpoolTimer([](PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_TIMER Timer) {
            if (focusedWidget)
                focusedWidget->onTimer();

            }, nullptr, nullptr);

        FILETIME dueTime;

        ULONGLONG qwDueTime = -10000000LL;
        dueTime.dwHighDateTime = (DWORD)(qwDueTime >> 32);
        dueTime.dwLowDateTime = (DWORD)qwDueTime;

        SetThreadpoolTimer(timer, &dueTime, CARET_FLICKERING_TIME_MS, 0);

        return FALSE;
    }

    LRESULT Window::onPaint(HWND hWnd)
    {
        //PAINTSTRUCT ps;
        //HDC hdc = BeginPaint(hWnd, &ps);
		//FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 3));
        renderer->render(this);
        ValidateRect(hWnd, nullptr);
        
        //EndPaint(hWnd, &ps);

        return FALSE;
    }
};
