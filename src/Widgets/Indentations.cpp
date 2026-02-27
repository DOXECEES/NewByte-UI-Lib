// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "Indentations.hpp"

NbRect<int> applyPaddingToRect(const NbRect<int>& rect, const OldPadding& padding) noexcept
{
	return {
		rect.x + padding.left,
		rect.y + padding.top,
		rect.width - padding.left - padding.right,
		rect.height - padding.top - padding.bottom
	};
}

NbRect<int> applyLeftTopPaddingToRect(const NbRect<int>& rect, const OldPadding& padding) noexcept
{
	return {
		rect.x + padding.left,
		rect.y + padding.top,
		rect.width,
		rect.height
	};
}

NbRect<int> applyScaleOnlyPadding(const NbRect<int>& rect, const OldPadding& padding) noexcept
{
	return {
		rect.x,
		rect.y,
		rect.width + padding.left + padding.right,
		rect.height + padding.top + padding.bottom
	};

}

NbRect<int> applyHeightOnlyPaddingToRect(const NbRect<int>& rect, const OldPadding& padding) noexcept
{
	return {
		rect.x,
		rect.y,
		rect.width,
		rect.height - padding.bottom
	};
}

