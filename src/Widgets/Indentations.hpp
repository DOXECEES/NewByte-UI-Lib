#ifndef NBUI_SRC_WIDGETS_INDENTATIONS_HPP
#define NBUI_SRC_WIDGETS_INDENTATIONS_HPP

#include <Core.hpp>

#include <TypeTraits.hpp>


template<typename T, typename = nbstl::isArithmetic<T>>
struct Margin
{
    T top       = T(0);
    T bottom    = T(0);
    T left      = T(0);
    T right     = T(0);

    template<typename U>
    Margin<U> to() const noexcept
    {
        return Margin<U>
        {
            .top    = static_cast<U>(top),
            .bottom = static_cast<U>(bottom),
            .left   = static_cast<U>(left),
            .right  = static_cast<U>(right)
        };
    }
};

template<typename T, typename = nbstl::isArithmetic<T>>
struct Padding
{
    T top       = T(0);
    T bottom    = T(0);
    T left      = T(0);
    T right     = T(0);

    template<typename U>
    Padding<U> to() const noexcept
    {
        return Padding<U>
        {
            .top = static_cast<U>(top),
            .bottom = static_cast<U>(bottom),
            .left = static_cast<U>(left),
            .right = static_cast<U>(right)
        };
    }
};





struct OldPadding
{
	int left = 5;
	int top = 5;
	int right = 5;
	int bottom = 5;
};

NbRect<int> applyPaddingToRect(const NbRect<int>& rect, const OldPadding& padding) noexcept;
NbRect<int> applyLeftTopPaddingToRect(const NbRect<int>& rect, const OldPadding& padding) noexcept;
NbRect<int> applyScaleOnlyPadding(const NbRect<int>& rect, const OldPadding& padding) noexcept;
NbRect<int> applyHeightOnlyPaddingToRect(const NbRect<int>& rect, const OldPadding& padding) noexcept;

#endif
