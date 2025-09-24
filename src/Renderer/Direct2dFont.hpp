#ifndef NBUI_SRC_DIRECT2DFONT_HPP
#define NBUI_SRC_DIRECT2DFONT_HPP

#include "Font.hpp"
#include "Direct2dWrapper.hpp"


#include <memory>

inline DWRITE_FONT_STYLE toDWriteFontStyle(FontStyle style)
{
	switch (style)
	{
		case FontStyle::NORMAL:  return DWRITE_FONT_STYLE_NORMAL;
		case FontStyle::ITALIC:  return DWRITE_FONT_STYLE_ITALIC;
		case FontStyle::OBLIQUE: return DWRITE_FONT_STYLE_OBLIQUE;
	}
	return DWRITE_FONT_STYLE_NORMAL; 
}

inline FontStyle fromDWriteFontStyle(DWRITE_FONT_STYLE style)
{
	switch (style)
	{
		case DWRITE_FONT_STYLE_NORMAL:  return FontStyle::NORMAL;
		case DWRITE_FONT_STYLE_ITALIC:  return FontStyle::ITALIC;
		case DWRITE_FONT_STYLE_OBLIQUE: return FontStyle::OBLIQUE;
	}
	return FontStyle::NORMAL; 
}

inline DWRITE_FONT_WEIGHT toDWriteFontWeight(FontWeight w)
{
	switch (w)
	{
		case FontWeight::Thin:        return DWRITE_FONT_WEIGHT_THIN;
		case FontWeight::ExtraLight:  return DWRITE_FONT_WEIGHT_EXTRA_LIGHT;
		case FontWeight::Light:       return DWRITE_FONT_WEIGHT_LIGHT;
		case FontWeight::Normal:      return DWRITE_FONT_WEIGHT_NORMAL;
		case FontWeight::Medium:      return DWRITE_FONT_WEIGHT_MEDIUM;
		case FontWeight::SemiBold:    return DWRITE_FONT_WEIGHT_SEMI_BOLD;
		case FontWeight::Bold:        return DWRITE_FONT_WEIGHT_BOLD;
		case FontWeight::ExtraBold:   return DWRITE_FONT_WEIGHT_EXTRA_BOLD;
		case FontWeight::Black:       return DWRITE_FONT_WEIGHT_BLACK;
	}
	return DWRITE_FONT_WEIGHT_NORMAL;
}

inline FontWeight fromDWriteFontWeight(DWRITE_FONT_WEIGHT w)
{
	switch (w)
	{
		case DWRITE_FONT_WEIGHT_THIN:        return FontWeight::Thin;
		case DWRITE_FONT_WEIGHT_EXTRA_LIGHT: return FontWeight::ExtraLight;
		case DWRITE_FONT_WEIGHT_LIGHT:       return FontWeight::Light;
		case DWRITE_FONT_WEIGHT_NORMAL:      return FontWeight::Normal;
		case DWRITE_FONT_WEIGHT_MEDIUM:      return FontWeight::Medium;
		case DWRITE_FONT_WEIGHT_SEMI_BOLD:   return FontWeight::SemiBold;
		case DWRITE_FONT_WEIGHT_BOLD:        return FontWeight::Bold;
		case DWRITE_FONT_WEIGHT_EXTRA_BOLD:  return FontWeight::ExtraBold;
		case DWRITE_FONT_WEIGHT_BLACK:       return FontWeight::Black;
	}
	return FontWeight::Normal;
}

inline DWRITE_FONT_STRETCH toDWriteFontStretch(FontStretch s)
{
	switch (s)
	{
		case FontStretch::UltraCondensed: return DWRITE_FONT_STRETCH_ULTRA_CONDENSED;
		case FontStretch::ExtraCondensed: return DWRITE_FONT_STRETCH_EXTRA_CONDENSED;
		case FontStretch::Condensed:      return DWRITE_FONT_STRETCH_CONDENSED;
		case FontStretch::SemiCondensed:  return DWRITE_FONT_STRETCH_SEMI_CONDENSED;
		case FontStretch::Normal:         return DWRITE_FONT_STRETCH_NORMAL;
		case FontStretch::SemiExpanded:   return DWRITE_FONT_STRETCH_SEMI_EXPANDED;
		case FontStretch::Expanded:       return DWRITE_FONT_STRETCH_EXPANDED;
		case FontStretch::ExtraExpanded:  return DWRITE_FONT_STRETCH_EXTRA_EXPANDED;
		case FontStretch::UltraExpanded:  return DWRITE_FONT_STRETCH_ULTRA_EXPANDED;
		case FontStretch::Undefined:      return DWRITE_FONT_STRETCH_UNDEFINED;
	}
	return DWRITE_FONT_STRETCH_NORMAL;
}


inline FontStretch fromDWriteFontStretch(DWRITE_FONT_STRETCH s)
{
	switch (s)
	{
		case DWRITE_FONT_STRETCH_ULTRA_CONDENSED:	return FontStretch::UltraCondensed;
		case DWRITE_FONT_STRETCH_EXTRA_CONDENSED:	return FontStretch::ExtraCondensed;
		case DWRITE_FONT_STRETCH_CONDENSED:			return FontStretch::Condensed;
		case DWRITE_FONT_STRETCH_SEMI_CONDENSED:	return FontStretch::SemiCondensed;
		case DWRITE_FONT_STRETCH_NORMAL:			return FontStretch::Normal;
		case DWRITE_FONT_STRETCH_SEMI_EXPANDED:		return FontStretch::SemiExpanded;
		case DWRITE_FONT_STRETCH_EXPANDED:			return FontStretch::Expanded;
		case DWRITE_FONT_STRETCH_EXTRA_EXPANDED:	return FontStretch::ExtraExpanded;
		case DWRITE_FONT_STRETCH_ULTRA_EXPANDED:	return FontStretch::UltraExpanded;
		case DWRITE_FONT_STRETCH_UNDEFINED:			return FontStretch::Undefined;
	}
	return FontStretch::Normal;
}



class Direct2dFont
{
public:

	explicit Direct2dFont(const Font& fontParam) noexcept;

	const Microsoft::WRL::ComPtr<IDWriteTextFormat>& getTextFormat() const noexcept;
	const Font& getFont() const noexcept;

private:
	Font										font;
	Microsoft::WRL::ComPtr<IDWriteTextFormat>	textFormat;

};


#endif