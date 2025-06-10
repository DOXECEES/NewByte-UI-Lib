#ifndef SRC_WINDOWINTERFACE_WINDOWSTATE_HPP
#define SRC_WINDOWINTERFACE_WINDOWSTATE_HPP

#include "../Core.hpp"
#include "WindowCore.hpp"

namespace WindowInterface
{
    struct FrameSize
    {
        int right   = 5;
        int left    = 5;
        int top     = 30;
        int bot     = 5;
    };

    class WindowState
    {
    public:
        NbColor         color       = {45, 45, 45};
        NbSize<int>     size        = { 800, 600 };
        std::wstring    title       = L"Ui-Lib";
        FrameSize       frameSize;

    };

};

#endif