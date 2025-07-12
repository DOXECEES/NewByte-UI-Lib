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

    void TextEdit::onButtonClicked(const wchar_t symbol, SpecialKeyCode specialCode)
    {
        switch (symbol)
        {
        case 0x25:
            if(specialCode == SpecialKeyCode::CTRL)
            {
                decrementCaretPosOnWord();
            }
            else
            {
                decrementCaretPos();
            }
            break;
        case 0x27:
        {
            if(specialCode == SpecialKeyCode::CTRL)
            {
                incrementCaretPosOnWord();
            }
            else
            {
                incrementCaretPos();
            }
            break;
        }
        case 0x08:
        {
            if(specialCode == SpecialKeyCode::CTRL)
            {
                deleteWord();
            }
            else
            {
                deleteChar();
            }
            break;
        }
        case 0x2E:
        {
            deleteCharRight();
            break;
        }
        default:
            return;
        }

        isCaretVisible = true;
    }

    void TextEdit::onSymbolButtonClicked(const wchar_t symbol)
    {
        // make some type of validator
        
        switch (symbol)
        {
        case L'\r':
        case L'\b':
            return;
        default:
            break;
        }
        
        data.insert(caretPosition, 1, symbol);
        caretPosition++;
        isDataChanged = true;
        isCaretVisible = true;
    }

    void TextEdit::onTimer()
    {
        isCaretVisible = !isCaretVisible;
    }

    void TextEdit::decrementCaretPos() noexcept
    {
        if(caretPosition > 0) caretPosition--;
    }
    

    void TextEdit::incrementCaretPos() noexcept
    {
        if(caretPosition < data.length()) caretPosition++;
    }
    
    
    
    void TextEdit::decrementCaretPosOnWord() noexcept
    {
        if(caretPosition == 0) return;

        for (size_t i = caretPosition - 1; i > 0; --i)
        {
            if (data[i] == L' ' && iswalnum(data[i - 1]))
            {
                caretPosition = i;
                return;
            }
        }

        caretPosition = 0;

    }
    void TextEdit::incrementCaretPosOnWord() noexcept
    {
        size_t len = data.length();

        if(len == 0) return;

        for (size_t i = caretPosition; i < len - 1; ++i)
        {
            if (data[i] == L' ' && iswalnum(data[i + 1]))
            {
                caretPosition = i + 1;
                return;
            }
        }

        caretPosition = len;
    }
    void TextEdit::deleteChar() noexcept
    {
        if(caretPosition > 0 )
        {
            data.erase(caretPosition - 1, 1);
            caretPosition--;
            isDataChanged = true;
        }
    }
    void TextEdit::deleteCharRight() noexcept
    {
        if(caretPosition < data.length())
        {
            data.erase(caretPosition, 1);
            isDataChanged = true;
        }
    }
    void TextEdit::deleteWord() noexcept
    {
        // if(caretPosition == 0) return;

        // std::function<bool(wchar_t)> comp;

        // if(iswalnum(data[caretPosition - 1]))
        // {
        //     comp = iswspace;
        // }
        // else
        // {
        //     comp = iswalnum;
        // }

        // for (int i = caretPosition; i >= 0;  i--)
        // {
        //     if(comp(data[i]))
        //     {
        //         data.erase(i, caretPosition - i);
        //         caretPosition = i;
        //         isDataChanged = true;
        //         return;
        //     }
        // }
    }


};