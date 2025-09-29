#ifndef NBUI_SRC_WIDGETS_INDENTATIONS_HPP
#define NBUI_SRC_WIDGETS_INDENTATIONS_HPP

#include <Core.hpp>

struct Padding
{
	int left = 5;
	int top = 5;
	int right = 5;
	int bottom = 5;
};

NbRect<int> applyPaddingToRect(const NbRect<int>& rect, const Padding& padding) noexcept;
NbRect<int> applyLeftTopPaddingToRect(const NbRect<int>& rect, const Padding& padding) noexcept;
NbRect<int> applyScaleOnlyPadding(const NbRect<int>& rect, const Padding& padding) noexcept;
NbRect<int> applyHeightOnlyPaddingToRect(const NbRect<int>& rect, const Padding& padding) noexcept;

#endif
