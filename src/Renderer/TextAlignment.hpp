#ifndef NBUI_SRC_RENDERER_TEXTALIGNMENT_HPP
#define NBUI_SRC_RENDERER_TEXTALIGNMENT_HPP

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
};

#endif