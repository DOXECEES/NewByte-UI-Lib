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
        int top     = 35;
        int bot     = 5;
    };

    class WindowState
    {
    public:
        NbColor         color       = { 45, 45, 45 };
        NbColor         frameColor  = { 35, 35, 35 };
        NbSize<int>     size        = { 800, 600 };
        std::wstring    title       = L"Ui-Lib";
        NbColor         fontColor   = { 255, 255, 255 };
        FrameSize       frameSize;
        NbSize<int>     minSize    = { 400, 300 };


    };

};

#endif