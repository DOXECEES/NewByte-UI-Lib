#ifndef NBUI_SRC_WIDGETS_LABEL_HPP
#define NBUI_SRC_WIDGETS_LABEL_HPP

#include "IWidget.hpp"
#include "Renderer/Font.hpp"

#include "Signal.hpp"

#include <Alghorithm.hpp>

namespace Widgets
{

    enum class TextAlign
    {
        CENTER,
        LEFT,
        RIGHT
    };

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
        explicit Label(const std::wstring& text) noexcept : IWidget({}), text(text) {}
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

        NbSize<int> computeContentSize() const noexcept override
        {
            constexpr int charWidth = 50;
            constexpr int lineHeight = 20;
            constexpr int paddingLeft = 4;
            constexpr int paddingRight = 4;
            constexpr int paddingTop = 2;
            constexpr int paddingBottom = 2;

            int width = static_cast<int>(text.size()) * charWidth + paddingLeft + paddingRight;
            int height = lineHeight + paddingTop + paddingBottom;

            return { width, height };
        }

        virtual const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
        {
            constexpr int charWidth = 50;
            constexpr int lineHeight = 20;
            constexpr int paddingLeft = 4;
            constexpr int paddingRight = 4;
            constexpr int paddingTop = 2;
            constexpr int paddingBottom = 2;

            int width = static_cast<int>(text.size()) * charWidth + paddingLeft + paddingRight;
            int height = lineHeight + paddingTop + paddingBottom;

            width = (nbstl::min)(width, maxSize.width);
            height = (nbstl::min)(height, maxSize.height);

            size.width = width;
            size.height = height;

            return size;
        }

        virtual void layout(const NbRect<int>& rect) noexcept override
        {
            this->rect = rect;
        }
        
    public:
        Signal<void(TextWrap)> onWrapChanged;
        Signal<void(const std::wstring&)> onTextChanged;

    private:
        std::wstring    text;
        TextWrap        wrap                = TextWrap::NONE;

        VTextAlign      verticalTextAlign   = VTextAlign::CENTER;
        HTextAlign      horizontalTextAlign = HTextAlign::CENTER;

        Font            font                = { };

        bool            isEllipsisSet       = false;

        NbSize<int>     size;
        
    };
};

#endif