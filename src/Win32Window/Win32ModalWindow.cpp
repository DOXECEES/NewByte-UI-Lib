// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "Win32ModalWindow.hpp"
#include "../Renderer/Direct2dRenderer.hpp"

namespace Win32Window
{
    ModalWindow::ModalWindow(IWindow *parent)
        :IWindow(), parent(parent)
    {
        state.size = {800, 600};

        WNDCLASS wc = {};
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = ModalWindow::staticWndProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = L"Modal";
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

        if (!RegisterClass(&wc))
        {
            ///MessageBox(nullptr, L"Failed to register class", L"Error", MB_OK);
        }

        HWND _handle = CreateWindow(L"Modal", L"NbWindow", WS_OVERLAPPEDWINDOW | WS_POPUP | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, state.size.width, state.size.height, parent->getHandle().as<HWND>(), nullptr,nullptr, this);
        handle = NbWindowHandle::fromWinHandle(_handle);
        WindowInterface::WindowMapper::registerWindow(handle, this);
       
        // LONG style = GetWindowLong(handle.as<HWND>(), GWL_STYLE);
        // style &= ~(WS_CAPTION);
        // SetWindowLong(handle.as<HWND>(), GWL_STYLE, style);

        // SetWindowPos(handle.as<HWND>(), nullptr, 0, 0, 0, 0,
        //          SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);       

        renderer = new Renderer::Direct2dRenderer(this);
        std::vector<CaptionButton>  captionButtons =
        {
            { L"✕", 50, 35, {240, 7, 23} },
            { L"🗖", 30, 35 },
            { L"🗕", 30, 35 },
        };

       captionButtons[0].setFunc([this]()
        {
            ShowWindow(handle.as<HWND>(), false);

        });
        captionButtons[1].setFunc([this]()
        {  
            ShowWindow(handle.as<HWND>(), false);

        });
        captionButtons[2].setFunc([this]()
        {
            ShowWindow(handle.as<HWND>(), false);
        });

        //widgets.push_back(new Widgets::Button(NbRect<int>(100, 100, 100, 100)));
        
        captionButtonsContainer.addButton(captionButtons[0]);
        captionButtonsContainer.addButton(captionButtons[1]);
        captionButtonsContainer.addButton(captionButtons[2]);
    }

    ModalWindow::~ModalWindow()
    {
        WindowInterface::WindowMapper::unregisterWindow(handle);
        delete renderer;
    }

    void ModalWindow::show()
    {
        ShowWindow(handle.as<HWND>(), TRUE);
        UpdateWindow(handle.as<HWND>());
    }

	void ModalWindow::repaint() const noexcept
	{
		InvalidateRect(handle.as<HWND>(), nullptr, TRUE);
		UpdateWindow(handle.as<HWND>());
	}



};
