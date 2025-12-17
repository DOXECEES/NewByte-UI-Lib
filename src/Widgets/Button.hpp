#ifndef NBUI_SRC_WIDGETS_BUTTON_HPP
#define NBUI_SRC_WIDGETS_BUTTON_HPP

#include <Alghorithm.hpp>

#include "IWidget.hpp"

#include "WidgetStyle.hpp"
#include "Theme.hpp"


namespace Widgets
{
    class Button : public IWidget
    {
    public:

        static constexpr const char* CLASS_NAME = "Button";

        Button() noexcept : IWidget({}) {};
        Button(const NbRect<int>& rect) : IWidget(rect) {};
        
        bool hitTest(const NbPoint<int>& pos) override; // 
        virtual bool hitTestClick(const NbPoint<int>& pos) noexcept override;

        virtual const char* getClassName() const override { return CLASS_NAME; }
        NB_NODISCARD const ButtonStyle& getButtonStyle() const noexcept;

        WidgetStyle& getStyle() noexcept override { return buttonStyle.baseStyle; }
        const WidgetStyle& getStyle() const noexcept override { return buttonStyle.baseStyle; }

        inline void setText(const std::wstring& text) noexcept { this->text = text; }
        inline const std::wstring& getText() const noexcept { return text; }
        
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

		//Signal<void()> onButtonClickedSignal;
        //Signal<void()> onButtonReleasedSignal;

    private:
        std::wstring    text;

        ButtonStyle     buttonStyle = ThemeManager::getCurrent().buttonStyle;
        NbSize<int>     size;
    };
}

#endif