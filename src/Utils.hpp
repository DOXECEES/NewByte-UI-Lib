#ifndef NBUI_SRC_UTILS_HPP
#define NBUI_SRC_UTILS_HPP

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

    template <typename T>
    constexpr NbSize<T> toNbSize(const SIZE &size) noexcept
    {
        return NbSize<T>(static_cast<T>(size.cx), static_cast<T>(size.cy));
    }

    template <typename T>
    constexpr NbPoint<T> toNbPoint(const POINT &point) noexcept
    {
        return NbPoint<T>(static_cast<T>(point.x), static_cast<T>(point.y));
    }

    template <typename T>
    constexpr NbRect<T> toNbRect(const RECT &rect) noexcept
    {
        return NbRect<T>(
            static_cast<T>(rect.left),
            static_cast<T>(rect.top),
            static_cast<T>(rect.right - rect.left),
            static_cast<T>(rect.bottom - rect.top)
        );
    }

     template <typename T>
    constexpr NbRect<T> toNbRect(const D2D1_RECT_F &rect) noexcept
    {
        return NbRect<T>(
            static_cast<T>(rect.left),
            static_cast<T>(rect.top),
            static_cast<T>(rect.right - rect.left),
            static_cast<T>(rect.bottom - rect.top)
        );
    }
    

};

#endif