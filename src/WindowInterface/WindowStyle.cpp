// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "WindowState.hpp"
#include "WindowStyle.hpp"

namespace WindowInterface
{
    void WindowStyle::setBorderRadius(const int radius) noexcept
    {
        borderRadius = radius;
    }

    int WindowStyle::getBorderRadius() const noexcept
    {
        return borderRadius;
    }
};

