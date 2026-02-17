#include "TextEdit.hpp"

#include <Alghorithm.hpp>

//TEMP
#include <Debug.hpp>
//

namespace Widgets
{
    TextEdit::TextEdit() noexcept
        :IWidget({})
    {
        onUnfocusedSignal.connect([this]() {
            this->isCaretVisible = false;
            Debug::debug("Unfocused signal");
        });
    }

    TextEdit::TextEdit(const NbRect<int>& rect) noexcept
        :IWidget(rect)
    {
        onUnfocusedSignal.connect([this]() {
            this->isCaretVisible = false;
            Debug::debug("Unfocused signal");
        });
    }

    bool TextEdit::hitTest(const NbPoint<int> &pos)
    {
        return pos.x >= rect.x && pos.x < rect.x + rect.width && pos.y >= rect.y && pos.y < rect.y + rect.height;
    }

    void TextEdit::onClick()
    {
    }

    void TextEdit::onButtonClicked(const wchar_t symbol, SpecialKeyCode specialCode)
    {
        wchar_t effectiveSymbol = symbol;

        // Если включен RTL, инвертируем физические стрелки для логического движения
        // 0x25 - Left, 0x27 - Right
        if (isRTL)
        {
            if (symbol == 0x25) effectiveSymbol = 0x27;
            else if (symbol == 0x27) effectiveSymbol = 0x25;
        }


        switch (symbol)
        {
        case 0x25: // Движение назад по строке
            if (specialCode == SpecialKeyCode::CTRL) decrementCaretPosOnWord();
            else decrementCaretPos();
            break;
        case 0x27: // Движение вперед по строке
            if (specialCode == SpecialKeyCode::CTRL) incrementCaretPosOnWord();
            else incrementCaretPos();
            break;
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
        switch (symbol)
        {
        case L'\r':
        case L'\b':
            return;
        default:
            break;
        }

        std::wstring testData = data;
        testData.insert(caretPosition, 1, symbol);

        auto res = validator.validate(testData);
        if (!res.isValid())
        {
            return;
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

    void TextEdit::setData(const std::wstring& data) noexcept
    {
        this->data = data;
        caretPosition = data.length();
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


    const NbSize<int>& TextEdit::measure(const NbSize<int>& maxSize) noexcept
    {
        // Да, это та самая временная переменная. 
        // В IWidget уже есть isSizeChange и rect, так что особо не хочется городить лишнего.
        // Но measure по контракту должен вернуть ссылку.
        static NbSize<int> measured;

        // Паддинги. Их желательно вытащить из WidgetStyle.
        const int paddingLeft = 0;
        const int paddingRight = 0;
        const int paddingTop = 0;
        const int paddingBottom = 0;

        // Измеряем ширину текста
        NbSize<int> textSize = { 0, 0 };
        if (!data.empty())
        {
            // Твой движок должен уметь измерять текст (в твоём TextMetrics или что-то похожее)
            textSize = { 80,30 };
        }

        // Минимальный размер: текст + отступы
        int width = textSize.width + paddingLeft + paddingRight;
        int height = nbstl::max(textSize.height, 20) + paddingTop + paddingBottom;

        // Ограничиваем maxSize
        measured.width = nbstl::min(width, maxSize.width);
        measured.height = nbstl::min(height, maxSize.height);

        return measured;
    }

    void TextEdit::layout(const NbRect<int>& newRect) noexcept
    {
        if (rect.width != newRect.width || rect.height != newRect.height)
        {
            isDataChanged = true; 
        }
        rect = newRect;
        isSizeChange = true;

        if (caretPosition > data.size())
        {
            caretPosition = static_cast<uint32_t>(data.size());
        }
    }


    void TextEdit::addValidator(Utils::Validator valid) noexcept
    {
        validator = valid;
    }

    void TextEdit::setIsRTL(bool rtl) noexcept
    {
        isRTL = rtl;
        isDataChanged = true;
    }

};