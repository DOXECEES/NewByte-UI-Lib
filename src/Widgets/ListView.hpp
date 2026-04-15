#pragma once

#include "IWidget.hpp"
#include <optional>
#include <string>
#include <vector>

namespace Widgets
{

    class ListView : public IWidget
    {
    public:
        static constexpr const char* CLASS_NAME              = "ListView";
        static constexpr int         HEIGHT_OF_ITEM_IN_PIXEL = 25;

        ListView() noexcept : IWidget({})
        {
            range.first = 0;
   
        }

        ListView(const NbRect<int>& rect) noexcept : IWidget(rect)
        {
            // range.first - смещение прокрутки (scroll offset)
            // range.second - высота видимой области
            range.first  = 0;
            range.second = rect.height;
        }

        const char* getClassName() const override
        {
            return CLASS_NAME;
        }

        // Добавление элементов
        void addItem(const std::string& text)
        {
            items.push_back(text);
        }

        void clear()
        {
            items.clear();
            selectedIndex = std::nullopt;
            range.first   = 0;
        }

        std::string getItemText(size_t index) const
        {
            if (index < items.size())
            {
                return items[index];
            }
            return "";
        }

        size_t getCount() const
        {
            return items.size();
        }

        std::optional<size_t> getSelectedIndex() const
        {
            return selectedIndex;
        }

        // Хит-тест для определения индекса под мышью
        bool hitTest(const NbPoint<int>& pos) override
        {
            if (!rect.isInside(pos))
            {
                lastHitIndex = -1;
                return false;
            }

            int localY    = pos.y - rect.y;
            int absoluteY = localY + range.first;
            lastHitIndex  = absoluteY / HEIGHT_OF_ITEM_IN_PIXEL;

            if (lastHitIndex >= (int)items.size())
            {
                lastHitIndex = -1;
                return false;
            }

            return true;
        }

        bool hitTestClick(const NbPoint<int>& pos) noexcept override
        {
            if (!rect.isInside(pos))
            {
                return false;
            }

            int localY    = pos.y - rect.y;
            int absoluteY = localY + range.first;
            int row       = absoluteY / HEIGHT_OF_ITEM_IN_PIXEL;

            if (row >= 0 && row < (int)items.size())
            {
                selectedIndex = row;
                onItemSelectedSignal.emit(row);
                onItemClickSignal.emit(row);
                return true;
            }

            return false;
        }

        // Обработка колеса мыши для прокрутки (как пример расширения функционала)
        void onMouseWheel(int delta) noexcept
        {
            int maxScroll =
                std::max(0, (int)(items.size() * HEIGHT_OF_ITEM_IN_PIXEL) - rect.height);
            range.first = std::clamp(range.first - (delta * 20), 0, maxScroll);
        }

        void layout(const NbRect<int>& newRect) noexcept override
        {
            setRect(newRect);
            range.second = newRect.height;
        }

        const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
        {
            measuredSize = {maxSize.width, maxSize.height};
            return measuredSize;
        }

        int getScrollOffset() const
        {
            return range.first;
        }
        int getLastHitIndex() const
        {
            return lastHitIndex;
        }

    public:
        // Сигналы в вашем стиле
        Signal<void(int)> onItemSelectedSignal;
        Signal<void(int)> onItemClickSignal;

    private:
        std::vector<std::string> items;
        std::optional<size_t>    selectedIndex;

        int                 lastHitIndex = -1;
        std::pair<int, int> range; // {scrollOffset, visibleHeight}
        NbSize<int>         measuredSize;
    };

} // namespace Widgets