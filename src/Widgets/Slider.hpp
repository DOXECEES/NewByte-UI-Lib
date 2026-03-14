#ifndef NBUI_SRC_WIDGETS_SLIDER_HPP
#define NBUI_SRC_WIDGETS_SLIDER_HPP

#include "Widgets/IWidget.hpp"
#include "GlobalWidgetContext.hpp"
#include <functional>
#include <algorithm>

namespace Widgets
{
    template <typename T>
    class Slider : public IWidget
    {
    public:
        using ValueGetter = std::function<T()>;
        using ValueSetter = std::function<void(T)>;

        DECLARE_WIDGET_CLASS_NAME(Slider);

        Slider() : IWidget({}) {}

        // --- БИНДИНГ ---
        // Позволяет связать слайдер с любой функцией/переменной
        void bind(ValueGetter getter, ValueSetter setter)
        {
            this->getter = getter;
            this->setter = setter;
            // Синхронизируем начальное значение
            if (this->getter)
            {
                value = this->getter();
            }
        }

        // Настройка диапазона
        void setRange(T minVal, T maxVal, T stepVal)
        {
            minValue = minVal;
            maxValue = maxVal;
            step = stepVal;
        }

        bool hitTest(const NbPoint<int>& pos) override
        {            
            return rect.isInside(pos);
        }

        const char* getClassName() const override
        {
            return CLASS_NAME;
        }

        T getValue() const noexcept
        {
            // Если есть внешний геттер, берем из него, иначе локальное
            return getter ? getter() : value;
        }

        void setValue(T newVal)
        {
            // Ограничиваем значение диапазоном
            T clampedValue = std::clamp(newVal, minValue, maxValue);
            
            value = clampedValue;

            // Вызываем внешний сеттер, если он привязан
            if (setter)
            {
                setter(value);
            }
        }

        void onClick() override
        { 
            isDragging = true;
            nbui::GlobalWidgetContext::captureWidget(this);
        }

        void onRelease() noexcept override
        {
            isDragging = false;
            nbui::GlobalWidgetContext::releaseWidget(this);
        }

        void onMouseMove(const MouseState& pos) noexcept override
        {
            if (isDragging)
            {
                T current = getValue();

                // Исправленная логика: направление мыши меняет значение на шаг
                if(pos.direction == MouseDirection::LEFT)
                {
                    setValue(current - step);
                }
                else if(pos.direction == MouseDirection::RIGHT)
                {
                    setValue(current + step);
                }
            }
        }

        virtual const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
        {
            measuredSize = {100, 20}; // Слайдеры обычно плоские и длинные
            return measuredSize;
        }

        virtual void layout(const NbRect<int>& rect) noexcept override
        {            
            this->rect = rect;
        }

    private:
        T value = static_cast<T>(0);
        T minValue = static_cast<T>(0);
        T maxValue = static_cast<T>(100);
        T step = static_cast<T>(0.1); // Шаг изменения
        
        bool isDragging = false;

        ValueGetter getter; 
        ValueSetter setter; 
    };
};

#endif