#include "Win32Window.hpp"

#include "../WindowInterface/WindowMapper.hpp"
#include "../Renderer/Direct2dRenderer.hpp"

namespace Win32Window
{
    Window::Window()
    {
        if(!registerWindowClass())
        {
            throw std::runtime_error("Failed to register window class.");
        }

        state.size = {800, 600};

        HWND _handle = CreateWindow(L"NbWindowClass", L"NbWindow", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, state.size.width, state.size.height, nullptr, nullptr, nullptr, this);
        handle = NbWindowHandle::fromWinHandle(_handle);
        WindowInterface::WindowMapper::registerWindow(handle, this);

        // renderer
        renderer = new Renderer::Direct2dRenderer(this);


        ShowWindow(handle.as<HWND>(), TRUE);
        UpdateWindow(handle.as<HWND>());

    }

    void Window::onSize(const NbSize<int>& newSize)
    {
        OutputDebugString(L"Window resized\n");
        state.size = std::move(newSize);

        renderer->resize(this);
    }

    bool Window::registerWindowClass()
    {
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
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
};
