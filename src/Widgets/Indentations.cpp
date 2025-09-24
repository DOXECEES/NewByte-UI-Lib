#include "Indentations.hpp"

NbRect<int> applyPaddingToRect(const NbRect<int>& rect, const Padding& padding) noexcept
{
	return {
		rect.x + padding.left,
		rect.y + padding.top,
		rect.width - padding.left - padding.right,
		rect.height - padding.top - padding.bottom
	};
}

NbRect<int> applyLeftTopPaddingToRect(const NbRect<int>& rect, const Padding& padding) noexcept
{
	return {
		rect.x + padding.left,
		rect.y + padding.top,
		rect.width,
		rect.height
	};
}
