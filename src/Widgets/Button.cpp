#include "Button.hpp"

void Widgets::Button::onClick()
{
    color = NbColor(105, 105, 105);
}

bool Widgets::Button::hitTest(const NbPoint<int> &pos)
{
    return pos.x >= rect.x && pos.x < rect.x + rect.width && pos.y >= rect.y && pos.y < rect.y + rect.height;
}
