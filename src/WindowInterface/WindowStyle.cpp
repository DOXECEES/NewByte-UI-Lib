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

