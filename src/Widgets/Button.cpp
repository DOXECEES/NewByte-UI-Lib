#include "Button.hpp"

namespace Widgets
{
    void Button::onClick() 
    {
        color = NbColor(105, 105, 105);
    }

    bool Button::hitTest(const NbPoint<int> &pos)
    {
        return pos.x >= rect.x && pos.x < rect.x + rect.width && pos.y >= rect.y && pos.y < rect.y + rect.height;
    }

};