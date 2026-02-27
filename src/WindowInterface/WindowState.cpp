// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "WindowState.hpp"

namespace WindowInterface
{
    void WindowState::setColor(const NbColor& newColor)
    {
        color = newColor;
    }

    void WindowState::setFrameColor(const NbColor &newFrameColor)
    {
        frameColor = newFrameColor;
    }

    void WindowState::setSize(const NbSize<int> &newSize)
    {
        size = newSize;
        isSizeChanged = true;

        if(size.width == 0 && size.height == 0)
        {
            return;
        }
        clientSize = { size.width - frameSize.left - frameSize.right, size.height - frameSize.top - frameSize.bot };

        clientRect.x = frameSize.left;
        clientRect.y = frameSize.top;
        clientRect.width = clientSize.width;
        clientRect.height = clientSize.height;

    }

    void WindowState::setTitle(const std::wstring &newTitle)
    {
        title = newTitle;
    }
    void WindowState::setFontColor(const NbColor &newFontColor)
    {
        fontColor = newFontColor;
    }

    void WindowState::setMinSize(const NbSize<int> &newMinSize)
    {
        minSize = newMinSize;
    }

    void WindowState::resetDirty()
    {
        isSizeChanged = false;
    }

    void WindowState::setClientRect(const NbRect<int>& newRect)
    {
        clientRect = {
            5 + newRect.x,
            35 + newRect.y,
            newRect.width - 5,
            newRect.height - 35
        };
    }

    void WindowState::calculateClientSize() {
        clientSize  = { size.width - frameSize.left - frameSize.right
                    , size.height - frameSize.top - frameSize.bot };
    }

    void WindowState::calculateClientRect()
    {
        clientRect = { frameSize.left, frameSize.top, clientSize.width, clientSize.height };
    }
};