#ifndef NBUI_SRC_WIDGETS_TEXTEDIT_HPP
#define NBUI_SRC_WIDGETS_TEXTEDIT_HPP

#include "IWidget.hpp"

namespace Widgets
{

    class TextEdit : public IWidget
    {
    public:

        TextEdit(const NbRect<int>& rect) : IWidget(rect) {};


        virtual const char* getClassName() const override { return "TextEdit"; }
        virtual bool hitTest(const NbPoint<int> &pos) override;

        virtual void onClick() override;
        virtual void onButtonClicked(const wchar_t symbol, SpecialKeyCode specialCode = SpecialKeyCode::NONE) override;
        virtual void onSymbolButtonClicked(const wchar_t symbol);
        virtual void onTimer() override;

        inline bool getIsDataChanged() const noexcept { return isDataChanged; }
        inline void resetIsDataChanged() noexcept { isDataChanged = false; }
        
        inline const std::wstring &getData() const noexcept { return data; }
        inline void setData(const std::wstring &data) noexcept { this->data = std::move(data); }

        inline bool getIsCaretVisible() const noexcept { return isCaretVisible; }   

        inline const uint32_t getCaretPos() const noexcept { return caretPosition; }
        void decrementCaretPos() noexcept;
        void incrementCaretPos() noexcept;

        void decrementCaretPosOnWord() noexcept;
        void incrementCaretPosOnWord() noexcept;

        void deleteChar() noexcept;
        void deleteCharRight() noexcept;

        void deleteWord() noexcept;

    private:
        std::wstring    data;

        bool            isDataChanged   = true; // only for renderer
        bool            isCaretVisible  = true;
        uint32_t        caretPosition   = 0;
    };
};

#endif