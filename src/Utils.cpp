// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "Utils.hpp"

namespace Utils
{
    bool isPointInsideRect(
        const NbPoint<int>& pos,
        RECT rect
    )
    {
        return rect.bottom > pos.y && rect.top < pos.y 
            && rect.left < pos.x && rect.right > pos.x;
    }

    int getWidthFromRect(const RECT rect) noexcept
    {
        return static_cast<int>(rect.bottom - rect.top);
    }

    int getHeightFromRect(const RECT rect) noexcept
    {
        return static_cast<int>(rect.right - rect.left);
    }

    D2D1::ColorF toD2D1Color(const NbColor& color) noexcept
    {
        return D2D1::ColorF(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
    }
}