#include "Label.hpp"

namespace Widgets
{

    const std::wstring& Label::getText() const noexcept
    {
        return text;
    }

    void Label::setText(const std::wstring &newText) noexcept
    {
        text = newText;
    }

    bool Label::hitTest(const NbPoint<int> &pos)
    {
        return pos.x >= rect.x && pos.x < rect.x + rect.width && pos.y >= rect.y && pos.y < rect.y + rect.height;
    }

};