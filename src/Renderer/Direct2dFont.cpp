#include "Direct2dFont.hpp"



Direct2dFont::Direct2dFont(const Font& fontParam) noexcept
	: font(fontParam)
{
	Microsoft::WRL::ComPtr<IDWriteFactory> directFactory = Renderer::FactorySingleton::getDirectWriteFactory();
	
	directFactory->CreateTextFormat(
		font.getName().c_str(),
		nullptr,
		toDWriteFontWeight(font.getWeight()),
		toDWriteFontStyle(font.getStyle()),
		toDWriteFontStretch(font.getStretch()),
		font.getSize(),
		L"en-us",
		&textFormat
	);

	textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

const Microsoft::WRL::ComPtr<IDWriteTextFormat>& Direct2dFont::getTextFormat() const noexcept
{
	return textFormat;
}

const Font& Direct2dFont::getFont() const noexcept
{
	return font;
}


