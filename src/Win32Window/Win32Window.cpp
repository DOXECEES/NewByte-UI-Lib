#include "Win32Window.hpp"

#include "../WindowInterface/WindowMapper.hpp"
#include "../Renderer/Direct2dRenderer.hpp"

#include "../Widgets/Button.hpp"

namespace Win32Window
{
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
        SetWindowLongPtr(handle.as<HWND>(), GWL_STYLE, GetWindowLongPtr(handle.as<HWND>(), GWL_STYLE) | WS_CLIPCHILDREN);


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
            if(isMaximized())
                ShowWindow(handle.as<HWND>(), SW_RESTORE);
            else
                ShowWindow(handle.as<HWND>(), SW_MAXIMIZE);
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
		delete renderer;
	}

	void Window::onSize(const NbSize<int>& newSize)
    {
        state.setSize(newSize);

        captionButtonsContainer.setPaintArea(NbRect<int>(0, 0, state.size.width, state.size.height));
        renderer->resize(this);
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
        WNDCLASSEX wcex;
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
};
