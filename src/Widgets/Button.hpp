#ifndef NBUI_SRC_WIDGETS_BUTTON_HPP
#define NBUI_SRC_WIDGETS_BUTTON_HPP

#include "IWidget.hpp"

namespace Widgets
{
    class Button : public IWidget
    {
    public:

        static constexpr const char* CLASS_NAME = "Button";

        Button(const NbRect<int>& rect) : IWidget(rect) {};
        
        bool hitTest(const NbPoint<int>& pos) override;

        virtual const char* getClassName() const override { return CLASS_NAME; }

        inline void setText(const std::wstring& text) noexcept { this->text = text; }
        inline const std::wstring& getText() const noexcept { return text; }
    
    private:
        std::wstring text;
        
    };
}

#endif