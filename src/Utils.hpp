#ifndef SRC_UTILS_HPP
#define SRC_UTILS_HPP

#include <Windows.h>

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

    bool isPointInsideRect(const NbPoint<int> pos, const RECT& rect)
    {
        return rect.bottom > pos.y && rect.top < pos.y 
                && rect.left < pos.x && rect.right > pos.x;
    }

    int getWidthFromRect(const RECT& rect) noexcept
    {
        return static_cast<int>(rect.bottom - rect.top);
    }

    int getHeightFromRect(const RECT& rect) noexcept
    {
        return static_cast<int>(rect.right - rect.left);
    }

    
};

#endif