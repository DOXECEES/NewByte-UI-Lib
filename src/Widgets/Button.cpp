#include "Button.hpp"

namespace Widgets
{
    bool Button::hitTest(const NbPoint<int> &pos)
    {
        return pos.x >= rect.x && pos.x < rect.x + rect.width && pos.y >= rect.y && pos.y < rect.y + rect.height;
    }

    const ButtonStyle& Button::getButtonStyle() const noexcept
    {
        return buttonStyle;
    }

};