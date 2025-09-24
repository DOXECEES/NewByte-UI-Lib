#ifndef NBUI_SRC_WIDGETS_LABEL_HPP
#define NBUI_SRC_WIDGETS_LABEL_HPP

#include "IWidget.hpp"
#include "Renderer/Font.hpp"

#include "Signal.hpp"

namespace Widgets
{
    class Label : public IWidget
    {

    public:

		enum class VTextAlign
		{
			CENTER,
			LEFT,
			RIGHT,
			JUSTIFY
		};

		enum class HTextAlign
		{
			TOP,
			BOTTOM,
			CENTER,
		};

		enum class TextWrap
		{
			NONE,
			CHAR,
			WORD
		};

        constexpr static const char* CLASS_NAME = "Label";

        Label() noexcept : IWidget({}) {}
        Label(const NbRect<int>& rect) : IWidget(rect) {}

        bool hitTest(const NbPoint<int> &pos) override;

        virtual const char *getClassName() const override { return CLASS_NAME; }

        void setEllipsis(bool enabled) noexcept;
        bool hasEllipsis() const noexcept;
        void toggleEllipsis() noexcept;

        const std::wstring& getText() const noexcept;
        void setText(const std::wstring &newText) noexcept;

        TextWrap getWrap() const noexcept;
        void setWrap(TextWrap textWrap) noexcept;

        void setVTextAlign(VTextAlign align) noexcept;
        VTextAlign getVTextAlign() const noexcept;

		void setHTextAlign(HTextAlign align) noexcept;
		HTextAlign getHTextAlign() const noexcept;

        void setFont(const Font& fontParam) noexcept;
        const Font& getFont() const noexcept;
        
    public:
        Signal<void(TextWrap)> onWrapChanged;

    private:
        std::wstring    text;
        TextWrap        wrap                = TextWrap::NONE;

        VTextAlign      verticalTextAlign   = VTextAlign::CENTER;
        HTextAlign      horizontalTextAlign = HTextAlign::CENTER;

        Font            font                = { };

        bool            isEllipsisSet       = false;

        
    };
};

#endif