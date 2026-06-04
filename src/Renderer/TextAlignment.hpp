#ifndef NBUI_SRC_RENDERER_TEXTALIGNMENT_HPP
#define NBUI_SRC_RENDERER_TEXTALIGNMENT_HPP

#include <dwrite.h>

enum class TextAlignment
{
    CENTER,
    LEFT,
    RIGHT,
    JUSTIFY
};

enum class ParagraphAlignment
{
    TOP,
    BOTTOM,
    CENTER,
};

struct TextFormatAlignment
{
    TextAlignment textAlignment = TextAlignment::CENTER;
    ParagraphAlignment paragraphAlignment = ParagraphAlignment::CENTER;
    int gap = 0;
};


DWRITE_TEXT_ALIGNMENT toDirect2dTextAlignment(TextAlignment align) noexcept;

DWRITE_PARAGRAPH_ALIGNMENT toDirect2dParagraphAlignment(ParagraphAlignment align) noexcept;


#endif