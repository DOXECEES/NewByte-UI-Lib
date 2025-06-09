//#include "MainWindow.hpp"

#include "Win32Window/Win32Window.hpp"
#include "Win32Window/Win32EventLoop.hpp"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
    Win32Window::Window window;
    Win32Window::Win32EventLoop eventLoop;
    eventLoop.run();

    // MainWindow window;
    // window.setTitle(L"UI-Lib test");
    // window.setBackgroundColor(NbColor(32, 32, 32));
    // window.setFrameColor(NbColor(46, 46, 46));
    // window.run();
}
