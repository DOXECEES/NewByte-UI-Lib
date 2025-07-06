#include "TextEdit.hpp"

namespace Widgets
{
    bool TextEdit::hitTest(const NbPoint<int> &pos)
    {
        return pos.x >= rect.x && pos.x < rect.x + rect.width && pos.y >= rect.y && pos.y < rect.y + rect.height;
    }
    void TextEdit::onClick()
    {
    }

    void TextEdit::onButtonClicked(const wchar_t symbol)
    {
        switch (symbol)
        {
        case 0x25:
            decrementCaretPos();
            break;
        case 0x27:
            incrementCaretPos();
            break;
        default:
            break;
        }
    }

    void TextEdit::onSymbolButtonClicked(const wchar_t symbol)
    {
        // make some type of validator
        
        if(symbol == L'\r')
            return;

        if(symbol == L'\b'&& caretPosition > 0)
        {
            data.erase(caretPosition - 1, 1);
            caretPosition--;
            isDataChanged = true;
            return;
        }    
        
        data.insert(caretPosition, 1, symbol);
        caretPosition++;
        isDataChanged = true;

    }

    void TextEdit::decrementCaretPos() noexcept
    {
        if(caretPosition > 0) caretPosition--;
    }
    void TextEdit::incrementCaretPos() noexcept
    {
        if(caretPosition < data.length()) caretPosition++;
    }
};