#include "MainWindow.hpp"


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
    MainWindow window;
    window.setTitle(L"UI-Lib test");
    window.setBackgroundColor(NbColor(32, 32, 32));
    window.setFrameColor(NbColor(46, 46, 46));
    window.run();
}
