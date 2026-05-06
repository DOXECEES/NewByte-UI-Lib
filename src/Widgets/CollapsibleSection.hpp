#ifndef SRC_LAYOUT_COLLAPSIBLESECTION_HPP
#define SRC_LAYOUT_COLLAPSIBLESECTION_HPP

#include "Layout/LayoutWidget.hpp"
#include "Widgets/Button.hpp" // Предполагаем наличие стандартной кнопки или кликабельного виджета

// В LayoutBuilder.hpp (уточненная версия)
namespace NNsLayout
{
    class CollapsibleLayout : public VLayout
    {
    public:
        CollapsibleLayout() noexcept
            : VLayout()
            , isCollapsed(false)
        {
        }

        void toggle() noexcept
        {
            setCollapsed(!isCollapsed);
        }

        void setCollapsed(bool collapsed) noexcept
        {
            if (isCollapsed != collapsed)
            {
                isCollapsed = collapsed;
                this->markDirty(); // Это запустит пересчет всей цепочки вверх
            }
        }

        bool getCollapsed() const noexcept
        {
            return isCollapsed;
        }

        void measure(const NbSize<int>& available) noexcept override
        {
            if (children.empty())
            {
                measuredSize = {0, 0};
                return;
            }

            // 1. Всегда измеряем Header (первый ребенок)
            children[0]->measure(available);
            NbSize<int> headerSize = children[0]->getMeasuredSize();

            if (isCollapsed)
            {
                // Если свернуто — размер равен только заголовку
                measuredSize = headerSize;
            }
            else
            {
                // Если развернуто — вызываем базовый замер VLayout (измерит всех детей)
                VLayout::measure(available);
            }
        }

        void layout(const NbRect<int>& bounds) noexcept override
        {
            if (isCollapsed)
            {
                layoutRect = bounds;
                if (!children.empty())
                {
                    // Размещаем только заголовок
                    NbRect<int> headerRect = bounds;
                    headerRect.height      = children[0]->getMeasuredSize().height;
                    children[0]->layout(headerRect);

                    // Остальных детей "схлопываем" в ноль, чтобы они не рисовались и не ловили
                    // клики
                    for (size_t i = 1; i < children.size(); ++i)
                    {
                        children[i]->layout({0, 0, 0, 0});
                    }
                }
            }
            else
            {
                // В развернутом состоянии работаем как обычный вертикальный стек
                VLayout::layout(bounds);
            }
        }

    private:
        bool isCollapsed;
    };
} // namespace NNsLayout

#endif