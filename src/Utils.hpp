#ifndef SRC_UTILS_HPP
#define SRC_UTILS_HPP

#include <Windows.h>

#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include "Core.hpp"

namespace Utils
{   
    template<typename T>
    void convertToWindowSpace(HWND handle, NbPoint<T>& point)
    {
        RECT rect;
        GetWindowRect(handle, &rect);
        point.x -= rect.left;
        point.y -= rect.top;
    }

    template<typename T>
    NbPoint<T> convertToWindowSpaceCopy(HWND handle, const NbPoint<T>& point)
    {
        NbPoint<T> ret = point;
        RECT rect;
        GetWindowRect(handle, &rect);
        ret.x -= rect.left;
        ret.y -= rect.top;
        return ret;
    }

    template<typename T>
    NbPoint<T> convertToScreenSpaceCopy(HWND handle, const NbPoint<T>& point)
    {
        NbPoint<T> ret = point;
        RECT rect;
        GetWindowRect(handle, &rect);
        ret.x += rect.left;
        ret.y += rect.top;
        return ret;
    }

    bool isPointInsideRect(const NbPoint<int> pos, const RECT &rect);

    int getWidthFromRect(const RECT &rect) noexcept;

    int getHeightFromRect(const RECT &rect) noexcept;

    D2D1::ColorF toD2D1Color(const NbColor &color) noexcept;
};

#endif