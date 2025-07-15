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
        clientSize = { size.width - frameSize.left - frameSize.right, size.height - frameSize.top - frameSize.bot };
        isSizeChanged = true;
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
};