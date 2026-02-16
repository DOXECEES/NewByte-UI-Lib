#ifndef NBUI_SRC_WIDGETS_SPINBOX_HPP
#define NBUI_SRC_WIDGETS_SPINBOX_HPP

#include "IWidget.hpp"
#include "Button.hpp"
#include "TextEdit.hpp"

#include <memory>

namespace Widgets
{
    class SpinBox : public IWidget
    {
    public:
     
        static constexpr const char* CLASS_NAME = "SpinBox";

        
        SpinBox(int initialValue = 0);

        const char* getClassName() const override { return CLASS_NAME; }

        void setRange(int minVal, int maxVal) noexcept;
        void setStep(int s) noexcept;

        void setValue(int v) noexcept;
        int getValue() const noexcept { return value; }

        const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override;
        void layout(const NbRect<int>& rect) noexcept override;

        virtual void onTimer() override;

        void onButtonClicked(const wchar_t symbol, SpecialKeyCode code) override;
        bool hitTest(const NbPoint<int>& pos) override;
        bool hitTestClick(const NbPoint<int>& pos) noexcept override;

        TextEdit* getInput() noexcept { return input.get(); }
        Button* getUpButton() noexcept { return upButton.get(); }
        Button* getDownButton() noexcept { return downButton.get(); }

        Signal<void(int)> onValueChanged;
        Signal<void(int)> onValueChangedByStep;

    private:
        // State
        int prevValue   = 1;
        int value       = 1;
        int minValue    = 1;
        int maxValue    = 100;
        int step        = 1;

        // Widgets
        std::unique_ptr<TextEdit> input;
        std::unique_ptr<Button> upButton;
        std::unique_ptr<Button> downButton;

        // Internal helpers
        void createInternalWidgets();
        void setupHandlers();

        void syncText();
        void onInputEdited();

        void onUpClicked();
        void onDownClicked();

        int clamp(int v) const noexcept
        {
            return (v < minValue) ? minValue : (v > maxValue ? maxValue : v);
        }

        void clampValue() noexcept { value = clamp(value); }

        NbSize<int> measuredSize = { 0, 0 };
    };
}

#endif
