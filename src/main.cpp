//#include "MainWindow.hpp"

#include "Win32Window/Win32Window.hpp"
#include "Win32Window/Win32EventLoop.hpp"

#include "Win32Window/Win32ModalWindow.hpp"

#include "Layout.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/TextEdit.hpp"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
    Win32Window::Window window;
    Win32Window::Win32EventLoop eventLoop;

    Layout *layout = new VBoxLayout(&window);

    Widgets::Button *button = new Widgets::Button(NbRect<int>(100, 100, 100, 100));
    button->setText(L"hello world");
    button->setOnClickCallback([&window]() 
    { 
        auto modalWindow = new Win32Window::ModalWindow(&window);
        modalWindow->show();

    });

    layout->addWidget(button);
    layout->addWidget(new Widgets::TextEdit(NbRect<int>(100, 100, 100, 100)));
    layout->addWidget(new Widgets::Button(NbRect<int>(100, 100, 100, 100)));
    layout->addWidget(new Widgets::Button(NbRect<int>(100, 100, 100, 100)));
    layout->addWidget(new Widgets::Button(NbRect<int>(100, 100, 100, 100)));
    layout->addWidget(new Widgets::Button(NbRect<int>(100, 100, 100, 100)));
    layout->addWidget(new Widgets::Button(NbRect<int>(100, 100, 100, 100)));

    window.show();

    eventLoop.run();

    delete layout;
    
    // MainWindow window;
    // window.setTitle(L"UI-Lib test");
    // window.setBackgroundColor(NbColor(32, 32, 32));
    // window.setFrameColor(NbColor(46, 46, 46));
    // window.run();
}
