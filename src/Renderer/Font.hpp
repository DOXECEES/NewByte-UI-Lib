#ifndef NBUI_SRC_RENDERER_FONT_HPP
#define NBUI_SRC_RENDERER_FONT_HPP

#include <string>

#include "NbCore.hpp"

enum class FontStyle
{
	NORMAL,
	OBLIQUE,
	ITALIC,
};

enum class FontWeight
{
	Thin,        // 100
	ExtraLight,  // 200
	Light,       // 300
	Normal,      // 400
	Medium,      // 500
	SemiBold,    // 600
	Bold,        // 700
	ExtraBold,   // 800
	Black,       // 900
};

enum class FontStretch
{
	Undefined,       // not set
	UltraCondensed,  // 1
	ExtraCondensed,  // 2
	Condensed,       // 3
	SemiCondensed,   // 4
	Normal,          // 5
	SemiExpanded,    // 6
	Expanded,        // 7
	ExtraExpanded,   // 8
	UltraExpanded    // 9
};


class Font
{
public:
	Font() = default;

	Font(
		std::wstring_view fontName,
		FontStyle style = FontStyle::NORMAL,
		FontWeight weight = FontWeight::Normal,
		FontStretch stretch = FontStretch::Normal,
		float size = 14.0f
	)
		: name(fontName), style(style), weight(weight), stretch(stretch), size(size)
	{}

	NB_NODISCARD const std::wstring& getName() const noexcept	{ return name; }
	NB_NODISCARD FontStyle getStyle() const noexcept			{ return style; }
	NB_NODISCARD FontWeight getWeight() const noexcept			{ return weight; }
	NB_NODISCARD FontStretch getStretch() const noexcept		{ return stretch; }
	NB_NODISCARD float getSize() const noexcept					{ return size; }

	void setName(std::wstring_view newName) noexcept;
	void setStyle(FontStyle newStyle) noexcept;
	void setWeight(FontWeight newWeight) noexcept;
	void setStretch(FontStretch newStretch) noexcept;
	void setSize(float newSize) noexcept;

	NB_NODISCARD bool isDirty() const noexcept;
	void clearDirty() const noexcept;

private:
	std::wstring    name		= L"Times New Roman";
	FontStyle       style		= FontStyle::NORMAL;
	FontWeight      weight		= FontWeight::Normal;
	FontStretch     stretch		= FontStretch::Normal;
	float           size		= 14.0f;

	mutable bool dirtyFlag		= true;
};
#endif