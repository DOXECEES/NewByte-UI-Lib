#include "Win32ChildWindow.hpp"

#include "../WindowInterface/WindowMapper.hpp"
#include "../Renderer/Direct2dRenderer.hpp"



namespace Win32Window
{
    ChildWindow::ChildWindow(WindowInterface::IWindow* parentWindow)
    {
        state.frameSize = {0, 0, 0, 0}; // no frame on clild window 

        WNDCLASS wc = {};
        wc.lpfnWndProc = staticWndProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.hbrBackground = nullptr;
        wc.style = CS_OWNDC;
        wc.lpszClassName = L"ChildWindow";
        RegisterClass(&wc);

        NbRect parentClientRect = parentWindow->getClientRect(); 
        state.setSize({400, 300});
        state.clientRect = { parentClientRect.x, parentClientRect.y, 400, 300 };

        HWND ihandle = CreateWindow(wc.lpszClassName, L"Child Window",
            WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
            state.clientRect.x, state.clientRect.y, state.clientRect.width, state.clientRect.height, parentWindow->getHandle().as<HWND>(), nullptr, wc.hInstance, this);
        handle = NbWindowHandle::fromWinHandle(ihandle);
        WindowInterface::WindowMapper::registerWindow(handle, this);
     
        renderer = new Renderer::Direct2dRenderer(this);
        state.setSize({ 0, 0 });

    }

	ChildWindow::~ChildWindow()
	{
        delete renderer;
	}



	void ChildWindow::show()
    {
        ShowWindow(handle.as<HWND>(), TRUE);
        UpdateWindow(handle.as<HWND>());
    }

	void ChildWindow::repaint() const noexcept
	{
        InvalidateRect(handle.as<HWND>(), nullptr, TRUE);
		UpdateWindow(handle.as<HWND>());
	}

	void ChildWindow::addCaption() noexcept
    {
        state.frameSize = FrameSize();
    }
}