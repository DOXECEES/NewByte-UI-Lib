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
        clientSize = {
            size.width - frameSize.left - frameSize.right - excludeSizes[ExcludeSide::RIGHT] -
                excludeSizes[ExcludeSide::LEFT],
            size.height - frameSize.top - frameSize.bot - excludeSizes[ExcludeSide::BOT] -
                excludeSizes[ExcludeSide::TOP]
        };

        clientRect.x = frameSize.left + excludeSizes[ExcludeSide::LEFT];
        clientRect.y = frameSize.top + excludeSizes[ExcludeSide::TOP];
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

    void WindowState::setExcludeFromTop(const int size)
    {
        excludeSizes[ExcludeSide::TOP] = size;
    }

    void WindowState::setClientRect(const NbRect<int>& newRect)
    {
        clientRect = {
            5 + newRect.x,
            35 + newRect.y + 32,
            newRect.width - 5,
            newRect.height - 35 - 32
        };
    }

    void WindowState::calculateClientSize() {
        clientSize = {
            size.width - frameSize.left - frameSize.right - excludeSizes[ExcludeSide::RIGHT] -
                excludeSizes[ExcludeSide::LEFT],
            size.height - frameSize.top - frameSize.bot - excludeSizes[ExcludeSide::BOT] -
                excludeSizes[ExcludeSide::TOP]
        };
    }

    void WindowState::calculateClientRect()
    {
        clientRect = {
            frameSize.left + excludeSizes[ExcludeSide::LEFT],
            frameSize.top + excludeSizes[ExcludeSide::TOP]
        , clientSize.width,
            clientSize.height
        };
    }
};