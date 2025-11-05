#ifndef NBUI_SRC_WIDGETS_BUTTON_HPP
#define NBUI_SRC_WIDGETS_BUTTON_HPP

#include "IWidget.hpp"

#include "WidgetStyle.hpp"
#include "Theme.hpp"

namespace Widgets
{
    class Button : public IWidget
    {
    public:

        static constexpr const char* CLASS_NAME = "Button";

        Button(const NbRect<int>& rect) : IWidget(rect) {};
        
        bool hitTest(const NbPoint<int>& pos) override;

        virtual const char* getClassName() const override { return CLASS_NAME; }
        NB_NODISCARD const ButtonStyle& getButtonStyle() const noexcept;

        inline void setText(const std::wstring& text) noexcept { this->text = text; }
        inline const std::wstring& getText() const noexcept { return text; }
    
    private:
        std::wstring    text;

        ButtonStyle     buttonStyle = ThemeManager::getCurrent().buttonStyle;
        
    };
}

#endif