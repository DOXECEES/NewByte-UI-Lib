#include "Label.hpp"

namespace Widgets
{

	void Label::setEllipsis(bool enabled) noexcept
	{
		isEllipsisSet = enabled;
	}

	bool Label::hasEllipsis() const noexcept
	{
		return isEllipsisSet;
	}

	void Label::toggleEllipsis() noexcept
	{
		isEllipsisSet = !isEllipsisSet;
	}

	const std::wstring& Label::getText() const noexcept
    {
        return text;
    }

    void Label::setText(const std::wstring &newText) noexcept
    {
        text = newText;
    }

	Label::TextWrap Label::getWrap() const noexcept
	{
        return wrap;
	}

	void Label::setWrap(TextWrap textWrap) noexcept
	{
        wrap = textWrap;
        onWrapChanged.emit(wrap);
	}

    void Label::setVTextAlign(VTextAlign align) noexcept
    {
        verticalTextAlign = align;
    }

	Label::VTextAlign Label::getVTextAlign() const noexcept
	{
        return verticalTextAlign;
	}

	void Label::setHTextAlign(HTextAlign align) noexcept
	{
        horizontalTextAlign = align;
	}

	Label::HTextAlign Label::getHTextAlign() const noexcept
	{
        return horizontalTextAlign;
	}

	void Label::setFont(const Font& fontParam) noexcept
	{
        font = fontParam;
	}

	const Font& Label::getFont() const noexcept
	{
		return font;
	}

	bool Label::hitTest(const NbPoint<int>& pos)
    {
        return pos.x >= rect.x && pos.x < rect.x + rect.width && pos.y >= rect.y && pos.y < rect.y + rect.height;
    }

};