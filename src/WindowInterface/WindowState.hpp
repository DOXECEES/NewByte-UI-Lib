#ifndef SRC_WINDOWINTERFACE_WINDOWSTATE_HPP
#define SRC_WINDOWINTERFACE_WINDOWSTATE_HPP

#include "../Core.hpp"
#include "WindowCore.hpp"

namespace WindowInterface
{
    class WindowState
    {
    public:
        NbColor     color   = {45, 45, 45};
        NbSize<int> size    = { 800, 600 };
        std::wstring title  = L"Ui-Lib";
    };
};

#endif