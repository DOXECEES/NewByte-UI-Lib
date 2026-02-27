#ifndef NBUI_SRC_WIDGETS_SPINBOX_HPP
#define NBUI_SRC_WIDGETS_SPINBOX_HPP

#include "Button.hpp"
#include "Core.hpp"
#include "IWidget.hpp"
#include "TextEdit.hpp"

#include <functional>
#include <iomanip>
#include <limits>
#include <memory>
#include <sstream>
#include <type_traits>

namespace Widgets
{



    class SpinBoxBase : public IWidget
    {
    public:
        static constexpr const char* CLASS_NAME = "SpinBox";

        SpinBoxBase() : IWidget({})
        {
        }

        virtual IWidget* getInput() const = 0;
        virtual IWidget* getUpButton() const = 0;
        virtual IWidget* getDownButton() const = 0;

        virtual ~SpinBoxBase() = default;
    };

 

    template <typename T> class SpinBox : public SpinBoxBase
    {
        static_assert(
            std::is_arithmetic_v<T>,
            "SpinBox requires arithmetic type"
        );

    public:
        static constexpr const char* CLASS_NAME = "SpinBox";

        SpinBox(T initialValue = T{}) : internalValue(initialValue)
        {
            createInternalWidgets();
            setupHandlers();
            syncText();
        }

        const char* getClassName() const override
        {
            return CLASS_NAME;
        }

       

        template <
            typename Getter,
            typename Setter>
        void bind(
            Getter g,
            Setter s
        )
        {
            getter = g;
            setter = s;
            syncText();
        }

        void bind(T* ptr)
        {
            getter = [ptr]()
            {
                return *ptr;
            };
            setter = [ptr](T v)
            {
                *ptr = v;
            };
            syncText();
        }

        void unbind()
        {
            getter = nullptr;
            setter = nullptr;
        }

       

        void setRange(
            T minVal,
            T maxVal
        ) noexcept
        {
            minValue = minVal;
            maxValue = maxVal;
            clampAndApply();
        }

        void setStep(T s) noexcept
        {
            step = s;
        }

        void setPrecision(int p) noexcept
        {
            if constexpr (std::is_floating_point_v<T>)
            {
                precision = p;
                syncText();
            }
        }

        T getValue() const noexcept
        {
            if (getter)
            {
                return getter();
            }

            return internalValue;
        }

        Signal<void(T)> onValueChanged;
        Signal<void(T)> onValueChangedByStep;

       

        const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
        {
            auto inputSize = input->measure(maxSize);
            int btnWidth = inputSize.height;

            measuredSize = {inputSize.width + btnWidth, inputSize.height};

            return measuredSize;
        }

        void layout(const NbRect<int>& rect) noexcept override
        {
            setRect(rect);

            int btnWidth = rect.height;

            NbRect<int> inputRect = rect;
            inputRect.width -= btnWidth;

            NbRect<int> upRect = {rect.x + inputRect.width, rect.y, btnWidth, rect.height / 2};

            NbRect<int> downRect
                = {rect.x + inputRect.width, rect.y + rect.height / 2, btnWidth, rect.height / 2};

            input->layout(inputRect);
            upButton->layout(upRect);
            downButton->layout(downRect);
        }

        bool hitTest(const NbPoint<int>& pos) override
        {
            return getRect().isInside(pos);
        }

        bool hitTestClick(const NbPoint<int>& pos) noexcept override
        {
            input->hitTestClick(pos);
            upButton->hitTestClick(pos);
            downButton->hitTestClick(pos);
            return false;
        }

    private:
   
        std::function<T()> getter;
        std::function<void(T)> setter;

        T internalValue{};

        T minValue = std::numeric_limits<T>::lowest();
        T maxValue = std::numeric_limits<T>::max();
        T step = static_cast<T>(1);

        int precision = 3;

        std::unique_ptr<TextEdit> input;
        std::unique_ptr<Button> upButton;
        std::unique_ptr<Button> downButton;

        NbSize<int> measuredSize{0, 0};

       

        void createInternalWidgets()
        {
            input = std::make_unique<TextEdit>(NbRect<int>{});
            upButton = std::make_unique<Button>(NbRect<int>{});
            downButton = std::make_unique<Button>(NbRect<int>{});

            upButton->setText(L"\u25B4");
            downButton->setText(L"\u25BE");

            addChildrenWidget(input.get());
            addChildrenWidget(upButton.get());
            addChildrenWidget(downButton.get());
        }

        void setupHandlers()
        {
            input->onUnfocusedSignal.connect(
                [this]()
                {
                    applyTextValue();
                }
            );

            upButton->onPressedSignal.connect(
                [this]()
                {
                    stepUp();
                }
            );

            downButton->onPressedSignal.connect(
                [this]()
                {
                    stepDown();
                }
            );
        }

        void stepUp()
        {
            T newValue = clamp(getValue() + step);
            applyValue(newValue);
            onValueChangedByStep.emit(newValue);
        }

        void stepDown()
        {
            T newValue = clamp(getValue() - step);
            applyValue(newValue);
            onValueChangedByStep.emit(newValue);
        }

        void applyTextValue()
        {
            std::wstring txt = input->getData();

            if (txt.empty())
            {
                syncText();
                return;
            }

            try
            {
                T parsed = parse(txt);
                applyValue(clamp(parsed));
            }
            catch (...)
            {
                syncText();
            }
        }

        void applyValue(T v)
        {
            T oldValue = getValue();

            if (setter)
            {
                setter(v);
            }
            else
            {
                internalValue = v;
            }

            if (oldValue != v)
            {
                onValueChanged.emit(v);
            }

            syncText();
        }

        void clampAndApply()
        {
            applyValue(clamp(getValue()));
        }

        void syncText()
        {
            input->setData(format(getValue()));
        }

        T clamp(T v) const noexcept
        {
            if (v < minValue)
            {
                return minValue;
            }
            if (v > maxValue)
            {
                return maxValue;
            }
            return v;
        }

        T parse(const std::wstring& txt) const
        {
            if constexpr (std::is_floating_point_v<T>)
            {
                return static_cast<T>(std::stof(txt));
            }
            else
            {
                return static_cast<T>(std::stoll(txt));
            }
        }

        std::wstring format(T v) const
        {
            if constexpr (std::is_floating_point_v<T>)
            {
                std::wstringstream ss;
                ss << std::fixed << std::setprecision(precision) << v;
                return ss.str();
            }
            else
            {
                return std::to_wstring(v);
            }
        }

        IWidget* getInput() const override
        {
            return input.get();
        }
        IWidget* getUpButton() const override
        {
            return upButton.get();
        }
        IWidget* getDownButton() const override
        {
            return downButton.get();
        }
    };

    using IntSpinBox = SpinBox<int>;
    using FloatSpinBox = SpinBox<float>;
    using DoubleSpinBox = SpinBox<double>;

}; 

#endif