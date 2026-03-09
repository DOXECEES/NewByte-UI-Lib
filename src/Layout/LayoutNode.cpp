// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "LayoutNode.hpp"

    #include "Widgets/IWidget.hpp"
    #include "WindowInterface/IWindow.hpp"
    #include <Alghorithm.hpp>

    #undef ABSOLUTE
    #undef RELATIVE

    namespace NNsLayout
    {
        void HLayout::measure(const NbSize<int>& available) noexcept
        {
            int totalFixed = 0;
            int maxHeight = 0;
            int flexCount = 0;

            for (auto& child : children)
            {
                auto& style = child->style;

                if (style.widthSizeType == SizeType::FLEX)
                {
                    flexCount++;
                    continue;
                }

                child->measure(available);
                auto s = child->getMeasuredSize();

                totalFixed += s.width;
                maxHeight = (std::max)(maxHeight, s.height);
            }

            measuredSize = { totalFixed, maxHeight };

        }

        void HLayout::layout(const NbRect<int>& bounds) noexcept
        {
            layoutRect = bounds;
            int x = bounds.x;

            float totalRelative = 0.0f;
            int totalFixed = 0;

            for (auto& child : children)
            {
                const NNsLayout::LayoutStyle& style = child->style;
                switch (style.widthSizeType)
                {
                    case SizeType::RELATIVE:
                    {
                        totalRelative += style.width;
                        break;
                    }
                    case SizeType::ABSOLUTE:
                    {
                        totalFixed += static_cast<int>(style.width);
                        break;
                    }
                    case SizeType::AUTO:
                    {
                        totalFixed += child->getMeasuredSize().width;
                        break;
                    }
                }
            }

            int remainingWidth = bounds.width - totalFixed;

            for (auto& child : children)
            {
                int width = 0;
                const NNsLayout::LayoutStyle& style = child->style;

                switch (style.widthSizeType)
                {
                    case SizeType::ABSOLUTE:
                    {
                        width = static_cast<int>(style.width);
                        break;
                    }
                    case SizeType::RELATIVE:
                    {
                        width = static_cast<int>(remainingWidth * (style.width / totalRelative));
                        break;
                    }
                    case SizeType::AUTO:
                    {
                        width = child->getMeasuredSize().width;
                        break;
                    }
                }

                int height = bounds.height;
                switch (style.heightSizeType)
                {
                    case SizeType::ABSOLUTE:
                    {
                        height = static_cast<int>(style.height);
                        break;
                    }
                    case SizeType::RELATIVE:
                    {
                        height = static_cast<int>(bounds.height * style.height);
                        break;
                    }
                    case SizeType::AUTO:
                    {
                        height = child->getMeasuredSize().height;
                        break;
                    }
                }

                NbRect<int> childRect;
                childRect.y = bounds.y + style.margin.top;
                childRect.height = bounds.height - (style.margin.top + style.margin.bottom);

                childRect.x = x + style.margin.left;
                childRect.width = width;

                child->setRect(childRect);
                child->layout(childRect);

                x += width + style.margin.left + style.margin.right;

            }
        }


        void VLayout::measure(const NbSize<int>& available) noexcept
        {
            int totalFixed = 0;
            float totalRelative = 0.0f;

            for (auto& child : children)
            {
                auto& st = child->style;

                if (st.heightSizeType == SizeType::RELATIVE)
                {
                    totalRelative += st.height;
                    continue;
                }

                child->measure(available);
                auto s = child->getMeasuredSize();

                int fullH =
                    st.margin.top + st.margin.bottom +
                    st.border.width * 2 +
                    st.padding.top + st.padding.bottom +
                    s.height;

                totalFixed += fullH;
            }

            int remaining = available.height - totalFixed;
            if (remaining < 0) remaining = 0;

            measuredSize.width = available.width;
            measuredSize.height = totalFixed;

            if (totalRelative > 0.0001f)
                measuredSize.height += remaining;
        }



        void VLayout::layout(const NbRect<int>& bounds) noexcept
        {
            layoutRect = bounds;

            float totalRelative = 0.0f;
            int totalFixed = 0;

            // ===== PASS 1 : MEASURE CHILDREN =====

            for (auto& child : children)
            {
                auto& st = child->style;

                if (st.heightSizeType == SizeType::AUTO)
                {
                    child->measure({bounds.width, bounds.height});
                }

                if (st.heightSizeType == SizeType::RELATIVE)
                {
                    totalRelative += st.height;
                }
                else if (st.heightSizeType == SizeType::ABSOLUTE)
                {
                    totalFixed += static_cast<int>(st.height) + st.margin.top + st.margin.bottom
                                  + st.border.width * 2 + st.padding.top + st.padding.bottom;
                }
                else if (st.heightSizeType == SizeType::AUTO)
                {
                    totalFixed += child->getMeasuredSize().height + st.margin.top + st.margin.bottom
                                  + st.border.width * 2 + st.padding.top + st.padding.bottom;
                }
            }

            // ===== PASS 2 : LAYOUT =====

            int remaining = bounds.height - totalFixed;
            if (remaining < 0)
            {
                remaining = 0;
            }

            int y = bounds.y;

            for (auto& child : children)
            {
                auto& st = child->style;

                int contentHeight = 0;

                if (st.heightSizeType == SizeType::ABSOLUTE)
                {
                    contentHeight = static_cast<int>(st.height);
                }
                else if (st.heightSizeType == SizeType::AUTO)
                {
                    contentHeight = child->getMeasuredSize().height;
                }
                else if (st.heightSizeType == SizeType::RELATIVE)
                {
                    if (totalRelative > 0.0001f)
                    {
                        contentHeight = static_cast<int>(remaining * (st.height / totalRelative));
                    }
                }

                int fullHeight = st.margin.top + st.border.width * 2 + st.padding.top
                                 + contentHeight + st.padding.bottom + st.border.width * 2
                                 + st.margin.bottom;

                NbRect<int> childRect;

                childRect.x = bounds.x + st.margin.left;
                childRect.y = y + st.margin.top;

                childRect.width = bounds.width - (st.margin.left + st.margin.right)
                                  ;

                childRect.height = contentHeight;

                child->setRect(childRect);
                child->layout(childRect);

                y += contentHeight + st.margin.top + st.margin.bottom;
            }
        }

        LayoutWidget::LayoutWidget(Widgets::IWidget* w) noexcept
            : LayoutNode(w),
              widget(w)
        {
            subscribe(
                widget.get(), &Widgets::IWidget::onSizeChangedSignal,
                [this](const NbRect<int>& rc)
                {
                    this->markDirty();
                }
            );
        }

        void LayoutWidget::setWidget(std::shared_ptr<Widgets::IWidget> w) noexcept
        {
            subscribe(
                *w, &Widgets::IWidget::onSizeChangedSignal,
                [&](const NbRect<int>&)
                {
                    dirty = true;
                }
            );
            widget = std::move(w);
            dirty = true;
        }

        void LayoutWidget::measure(const NbSize<int>& available) noexcept
        {
            //const auto natural = widget ? widget->computeContentSize() : NbSize<int>{ 0, 0 };
            const auto natural = widget->measure(available);

            int w = 0;
            int h = 0;

            switch (style.widthSizeType) {
            case SizeType::ABSOLUTE:
                w = static_cast<int>(style.width);
                break;
            case SizeType::RELATIVE:
                w = static_cast<int>(available.width * style.width);
                break;
            case SizeType::FLEX:
                w = 0;
                break;
            case SizeType::AUTO:
                w = natural.width;
                break;
            }

            switch (style.heightSizeType) {
            case SizeType::ABSOLUTE:
                h = static_cast<int>(style.height);
                break;
            case SizeType::RELATIVE:
                h = static_cast<int>(available.height * style.height);
                break;
            case SizeType::FLEX:
                h = 0;
                break;
            case SizeType::AUTO:
                h = natural.height;
                break;
            }

            int internalW = style.padding.left + style.padding.right + style.border.width * 2;
            int internalH = style.padding.top + style.padding.bottom + style.border.width * 2;

            w += internalW;
            h += internalH;

            // Ограничиваем размерами контента и min/max
            measuredSize = {
                (nbstl::max)(style.minSize.width, w), (nbstl::max)(style.minSize.height, h)
            };
        }

    
        void LayoutWidget::layout(const NbRect<int>& bounds) noexcept
        {
            if (!widget) return;

            NbRect<int> inner = bounds;


            inner.x += style.border.width;
            inner.y += style.border.width;
            inner.width  -= style.border.width * 2;
            inner.height -= style.border.width * 2;

            inner.x += style.padding.left;
            inner.y += style.padding.top;
            inner.width  -= style.padding.left + style.padding.right;
            inner.height -= style.padding.top  + style.padding.bottom;

            widget->layout(inner);
        }


        void LayoutWindow::layout(const NbRect<int>& bounds) noexcept
        {
            if (!ownerWindow)
            {
                return;
            }


            if (!children.empty())
            {
                NbRect<int> client = ownerWindow->getClientRect();

                children[0]->layout(client);
            }
        }

        void GridLayout::measure(const NbSize<int>& available) noexcept
        {
            if (children.empty() || columns <= 0)
            {
                measuredSize = {0, 0};
                return;
            }

            int maxChildW = 0;
            int maxChildH = 0;

            // Измеряем каждого ребенка и находим самую большую ячейку
            for (auto& child : children)
            {
                child->measure(available);
                auto s = child->getMeasuredSize();
                auto& st = child->style;

                int fullW = s.width + st.margin.left + st.margin.right;
                int fullH = s.height + st.margin.top + st.margin.bottom;

                maxChildW = (std::max)(maxChildW, fullW);
                maxChildH = (std::max)(maxChildH, fullH);
            }

            int numRows =
                (rows > 0) ? rows : (static_cast<int>(children.size()) + columns - 1) / columns;

            // Итоговый размер = размер ячейки * кол-во ячеек
            measuredSize.width = maxChildW * columns;
            measuredSize.height = maxChildH * numRows;

            // Не превышаем доступное место
            measuredSize.width = (std::min)(measuredSize.width, available.width);
            measuredSize.height = (std::min)(measuredSize.height, available.height);
        }

        void GridLayout::layout(const NbRect<int>& bounds) noexcept
        {
            layoutRect = bounds;
            if (children.empty() || columns <= 0)
            {
                return;
            }

            // 1. Вычисляем количество строк
            int totalChildren = static_cast<int>(children.size());
            int numRows = (rows > 0) ? rows : (totalChildren + columns - 1) / columns;

            // 2. Вычисляем ширину одной колонки и высоту одной строки
            // (Для простоты берем равномерное распределение,
            // но учитываем общие отступы контейнера)

            float cellW = static_cast<float>(bounds.width) / columns;
            float cellH = static_cast<float>(bounds.height) / numRows;

            int index = 0;
            for (auto& child : children)
            {
                int col = index % columns;
                int row = index / columns;

                const auto& st = child->style;

                // Определяем границы ячейки (Cell Rect)
                int cellX = bounds.x + static_cast<int>(col * cellW);
                int cellY = bounds.y + static_cast<int>(row * cellH);
                int currentCellW = static_cast<int>(cellW);
                int currentCellH = static_cast<int>(cellH);

                // 3. Учитываем Margin ребенка внутри ячейки
                NbRect<int> childRect;
                childRect.x = cellX + st.margin.left;
                childRect.y = cellY + st.margin.top;

                // Ширина = ширина ячейки минус внешние отступы
                childRect.width = currentCellW - (st.margin.left + st.margin.right);
                childRect.height = currentCellH - (st.margin.top + st.margin.bottom);

                // 4. Ограничиваем размеры, если у ребенка ABSOLUTE или AUTO
                if (st.widthSizeType == SizeType::ABSOLUTE)
                {
                    childRect.width = (std::min)(childRect.width, static_cast<int>(st.width));
                }
                if (st.heightSizeType == SizeType::ABSOLUTE)
                {
                    childRect.height = (std::min)(childRect.height, static_cast<int>(st.height));
                }

                // Устанавливаем и запускаем внутренний Layout
                child->setRect(childRect);
                child->layout(childRect);

                index++;
                if (rows > 0 && index >= columns * rows)
                {
                    break; // Лимит сетки
                }
            }
        }
    } // namespace NNsLayout
