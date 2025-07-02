//#include "MainWindow.hpp"

#include "Win32Window/Win32Window.hpp"
#include "Win32Window/Win32EventLoop.hpp"

#include "Layout.hpp"
#include "Widgets/Button.hpp"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{


    Win32Window::Window window;
    Win32Window::Win32EventLoop eventLoop;
    
    Layout *layout = new VBoxLayout(&window);
    layout->addWidget(new Widgets::Button(NbRect<int>(100, 100, 100, 100)));
    layout->addWidget(new Widgets::Button(NbRect<int>(100, 100, 100, 100)));
    layout->addWidget(new Widgets::Button(NbRect<int>(100, 100, 100, 100)));
    layout->addWidget(new Widgets::Button(NbRect<int>(100, 100, 100, 100)));
    layout->addWidget(new Widgets::Button(NbRect<int>(100, 100, 100, 100)));
    layout->addWidget(new Widgets::Button(NbRect<int>(100, 100, 100, 100)));
    layout->addWidget(new Widgets::Button(NbRect<int>(100, 100, 100, 100)));


    window.show();

    eventLoop.run();
    // MainWindow window;
    // window.setTitle(L"UI-Lib test");
    // window.setBackgroundColor(NbColor(32, 32, 32));
    // window.setFrameColor(NbColor(46, 46, 46));
    // window.run();
}
