// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "Button.hpp"

namespace Widgets
{
    bool Button::hitTest(const NbPoint<int> &pos)
    {
        return pos.x >= rect.x && pos.x < rect.x + rect.width && pos.y >= rect.y && pos.y < rect.y + rect.height;
    }

    bool Button::hitTestClick(const NbPoint<int>& pos) noexcept
    {
        bool isHit = hitTest(pos);
        if (isHit)
        {
            onClick();
        }
        return isHit;

    }

    const ButtonStyle& Button::getButtonStyle() const noexcept
    {
        return buttonStyle;
    }

};