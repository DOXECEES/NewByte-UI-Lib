#ifndef NBUI_SRC_WIDGETS_INDENTATIONS_HPP
#define NBUI_SRC_WIDGETS_INDENTATIONS_HPP

#include <Core.hpp>
#include <TypeTraits.hpp>

template<typename T, typename = nbstl::isArithmetic<T>>
struct Margin
{
    T top       = T(0);
    T right     = T(0);
    T bottom    = T(0);
    T left      = T(0);

    template<typename U>
    Margin<U> to() const noexcept
    {
        return Margin<U>
        {
            .top    = static_cast<U>(top),
            .right  = static_cast<U>(right),
            .bottom = static_cast<U>(bottom),
            .left   = static_cast<U>(left)
        };
    }
};

template<typename T, typename = nbstl::isArithmetic<T>>
struct Padding
{
    T top       = static_cast<T>(0);
    T right     = static_cast<T>(0);
    T bottom    = static_cast<T>(0);
    T left      = static_cast<T>(0);

    Padding() = default;

    Padding(T all)
        : top(all)
        , right(all)
        , bottom(all)
        , left(all)
    {
    }

    Padding(
        T v,
        T h
    )
        : top(v)
        , right(h)
        , bottom(v)
        , left(h)
    {
    }

    Padding(
        T t,
        T r,
        T b,
        T l
    )
        : top(t)
        , right(r)
        , bottom(b)
        , left(l)
    {
    }


    template <typename U>
    Padding<U> to() const noexcept
    {
        return Padding<U>(
            static_cast<U>(top),
            static_cast<U>(right),
            static_cast<U>(bottom),
            static_cast<U>(left)
        );
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
