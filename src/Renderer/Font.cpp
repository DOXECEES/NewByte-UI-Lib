#include "Font.hpp"

void Font::setName(std::wstring_view newName) noexcept
{
	name = newName;
	dirtyFlag = true;
}


void Font::setStyle(FontStyle newStyle) noexcept
{
	style = newStyle;
	dirtyFlag = true;
}


void Font::setWeight(FontWeight newWeight) noexcept
{
	weight = newWeight;
	dirtyFlag = true;
}


void Font::setStretch(FontStretch newStretch) noexcept
{
	stretch = newStretch;
	dirtyFlag = true;
}


void Font::setSize(float newSize) noexcept
{
	size = newSize;
	dirtyFlag = true;
}


NB_NODISCARD bool Font::isDirty() const noexcept
{
	return dirtyFlag;
}


void Font::clearDirty() const noexcept
{
	dirtyFlag = false;
}

