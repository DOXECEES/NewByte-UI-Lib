#ifndef NBUI_SRC_WIDGETS_TEXTEDIT_HPP
#define NBUI_SRC_WIDGETS_TEXTEDIT_HPP

#include "IWidget.hpp"

#include "Utils/Validator.hpp"

namespace Widgets
{

    class TextEdit : public IWidget
    {
    public:

        static constexpr const char* CLASS_NAME = "TextEdit";


        TextEdit() noexcept;
        explicit TextEdit(const NbRect<int>& rect) noexcept;


        virtual const char* getClassName() const override { return CLASS_NAME; }
        virtual bool hitTest(const NbPoint<int> &pos) override;

        virtual void onClick() override;
        virtual void onButtonClicked(const wchar_t symbol, SpecialKeyCode specialCode = SpecialKeyCode::NONE) override;
        virtual void onSymbolButtonClicked(const wchar_t symbol);
        virtual void onTimer() override;

        inline bool getIsDataChanged() const noexcept { return isDataChanged; }
        inline void resetIsDataChanged() noexcept { isDataChanged = false; }
        
        inline const std::wstring &getData() const noexcept { return data; }
        void setData(const std::wstring& data) noexcept;

        inline bool getIsCaretVisible() const noexcept { return isCaretVisible; }  
        inline void setIsCaretVisible(bool flag) noexcept { isCaretVisible = flag; }

        inline const uint32_t getCaretPos() const noexcept { return caretPosition; }
        void decrementCaretPos() noexcept;
        void incrementCaretPos() noexcept;

        void decrementCaretPosOnWord() noexcept;
        void incrementCaretPosOnWord() noexcept;

        void deleteChar() noexcept;
        void deleteCharRight() noexcept;

        void deleteWord() noexcept;

        const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override;
        void layout(const NbRect<int>& rect) noexcept override;

        void addValidator(Utils::Validator valid) noexcept;

        inline bool getIsRTL() const noexcept { return isRTL; }
        void setIsRTL(bool rtl) noexcept;
        

    private:
        Utils::Validator validator;
        std::wstring    data;

        bool            isDataChanged   = true; // only for renderer
        bool            isCaretVisible  = true;
        bool            isRTL           = false; 
        uint32_t        caretPosition   = 0;
    };
};

#endif