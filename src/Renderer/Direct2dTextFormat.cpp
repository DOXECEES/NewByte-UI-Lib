#include "Direct2dTextFormat.hpp"

#include "FactorySingleton.hpp"

namespace Renderer
{
    Direct2dTextFormat::Direct2dTextFormat(
        const Font& font,
        TextFormatAlignment alignment
    ) noexcept
    {
        auto writeFactory = FactorySingleton::getDirectWriteFactory();
        Direct2dFont direct2dFont(font);
        textFormat = direct2dFont.getTextFormat();

        switch (alignment.textAlignment)
        {
        case TextAlignment::CENTER:
            textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            break;
        case TextAlignment::LEFT:
            textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            break;
        case TextAlignment::RIGHT:
            textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
            break;
        case TextAlignment::JUSTIFY:
            textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
            break;
        }

        switch (alignment.paragraphAlignment)
        {
        case ParagraphAlignment::TOP:
            textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            break;
        case ParagraphAlignment::BOTTOM:
            textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
            break;
        case ParagraphAlignment::CENTER:
            textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            break;
        }

    }

    const Microsoft::WRL::ComPtr<IDWriteTextFormat>&
    Direct2dTextFormat::getTextFormat() const noexcept
    {
        return textFormat;
    }
}; 