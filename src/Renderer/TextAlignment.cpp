#include "TextAlignment.hpp"



DWRITE_TEXT_ALIGNMENT toDirect2dTextAlignment(TextAlignment align) noexcept
{
    switch (align)
    {
    case TextAlignment::LEFT:
        return DWRITE_TEXT_ALIGNMENT_LEADING;
    case TextAlignment::RIGHT:
        return DWRITE_TEXT_ALIGNMENT_TRAILING;
    case TextAlignment::CENTER:
        return DWRITE_TEXT_ALIGNMENT_CENTER;
    case TextAlignment::JUSTIFY:
        return DWRITE_TEXT_ALIGNMENT_JUSTIFIED;
    default:
        return DWRITE_TEXT_ALIGNMENT_LEADING;
    }
}


DWRITE_PARAGRAPH_ALIGNMENT toDirect2dParagraphAlignment(ParagraphAlignment align) noexcept
{
    switch (align)
    {
    case ParagraphAlignment::TOP:
        return DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
    case ParagraphAlignment::BOTTOM:
        return DWRITE_PARAGRAPH_ALIGNMENT_FAR;
    case ParagraphAlignment::CENTER:
        return DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
    default:
        return DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
    }
}