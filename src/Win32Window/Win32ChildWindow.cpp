#include "Win32ChildWindow.hpp"

#include "../WindowInterface/WindowMapper.hpp"

namespace Win32Window
{
    ChildWindow::ChildWindow(WindowInterface::IWindow* parentWindow)
    {
        WNDCLASS wc = {};
        wc.lpfnWndProc = staticWndProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.hbrBackground = nullptr;
        wc.style = CS_OWNDC;
        wc.lpszClassName = L"ChildWindow";
        RegisterClass(&wc);

        NbRect parentClientRect = parentWindow->getClientRect(); 
        state.clientRect = { parentClientRect.x, parentClientRect.y, 400, 300 };
        HWND ihandle = CreateWindowEx(0, wc.lpszClassName, L"Child Window",
            WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
            state.clientRect.x, state.clientRect.y, state.clientRect.width, state.clientRect.height, parentWindow->getHandle().as<HWND>(), nullptr, wc.hInstance, nullptr);
        handle = NbWindowHandle::fromWinHandle(ihandle);
        WindowInterface::WindowMapper::registerWindow(handle, this);
        
    }

    void ChildWindow::show()
    {
        ShowWindow(handle.as<HWND>(), TRUE);
        UpdateWindow(handle.as<HWND>());
    }
}