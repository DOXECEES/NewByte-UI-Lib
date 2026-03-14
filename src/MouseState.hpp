#ifndef NBUI_SRC_MOUSESTATE_HPP
#define NBUI_SRC_MOUSESTATE_HPP

#include "Core.hpp"

enum class MouseDirection
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};
struct MouseState
{
    NbPoint<int> position;
    MouseDirection direction;                   
};

#endif