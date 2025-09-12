#ifndef NBUI_SRC_WIDGETS_LABEL_HPP
#define NBUI_SRC_WIDGETS_LABEL_HPP

#include "IWidget.hpp"

namespace Widgets
{
    class Label : public IWidget
    {
        enum class VTextAlign
        {
            LEFT,
            CENTER,
            RIGHT,
            JUSTIFY
        };

        enum class HTextAlign
        {
            TOP,
            CENTER,
            BOTTOM,
            BASELINE
        };

        enum class TextWrap
        {
            NONE,
            CHAR,
            WORD
        };

    public:
        constexpr static const char* CLASS_NAME = "Label";


        Label(const NbRect<int>& rect) : IWidget(rect) {}

        bool hitTest(const NbPoint<int> &pos) override;

        virtual const char *getClassName() const override { return CLASS_NAME; }

        void setEllipsis(bool enabled) noexcept;

        const std::wstring& getText() const noexcept;
        void setText(const std::wstring &newText) noexcept;

    private:
        std::wstring text;
    };
};

#endif