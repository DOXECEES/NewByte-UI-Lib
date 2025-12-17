#include "Win32ChildWindow.hpp"

#include "../WindowInterface/WindowMapper.hpp"
#include "../Renderer/Direct2dRenderer.hpp"

#include "NewRenderer/Direct2dWindowRenderer.hpp"


namespace Win32Window
{
    ChildWindow::ChildWindow(WindowInterface::IWindow* parentWindow, bool setOwnDC)
    {
        state.frameSize = { 0, 0, 0, 0 }; // no frame on clild window 
        style.setBorderRadius(0);
        WNDCLASS wc = {};
        wc.lpfnWndProc = staticWndProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
        if (setOwnDC)
        {
            wc.style = CS_OWNDC;
        }
        else
        {
            wc.style = 0;
        }
        wc.lpszClassName = L"ChildWindow";
        RegisterClass(&wc);

        DWORD windowStyle = {};
        HWND parentHandle = nullptr;

        if (!parentWindow)
        {
            windowStyle = WS_POPUP | WS_VISIBLE;
            state.setSize({ 400, 300 });
            state.minSize = { 400, 300 };

            //state.clientRect = { state.frameSize.left, parentClientRect.y, 400, 300 };
            state.clientRect = NbRect<int>(state.frameSize.left
                , state.frameSize.top
                , 400 - state.frameSize.left - state.frameSize.right
                , 300 - state.frameSize.top - state.frameSize.bot);

        }
        else
        {
            parentHandle = parentWindow->getHandle().as<HWND>();
            windowStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
            windowStyle &= ~(CS_HREDRAW | CS_VREDRAW);

            NbRect parentClientRect = parentWindow->getClientRect();
            state.setSize({ 300, 200 });
            state.setMinSize({ 0,0 });
            state.clientRect = { parentClientRect.x + 50, parentClientRect.y + 50, 300, 200 };
        }

        HWND ihandle = CreateWindow(wc.lpszClassName, L"Child Window",
            windowStyle,
            state.clientRect.x, state.clientRect.y, state.clientRect.width, state.clientRect.height, parentHandle, nullptr, wc.hInstance, this);
        handle = NbWindowHandle::fromWinHandle(ihandle);
        WindowInterface::WindowMapper::registerWindow(handle, this);
     
        renderer = new Renderer::Direct2dRenderer(this);
        //state.setSize({ 0, 0 });

    }

	ChildWindow::~ChildWindow()
	{
        delete renderer;
	}

	void ChildWindow::show()
    {
        notifyAllListeners();
        ShowWindow(handle.as<HWND>(), TRUE);
        InvalidateRect(handle.as<HWND>(), nullptr, TRUE);
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
        /*state.clientRect = NbRect<int>(state.clientRect.x + state.frameSize.left
            , state.clientRect.y + state.frameSize.top
            , 400 - state.frameSize.left - state.frameSize.right
            , 300 - state.frameSize.top - state.frameSize.bot);*/

    }

    void ChildWindow::setRenderable(bool flag) noexcept
    {
        isRenderable = flag;
    }
}