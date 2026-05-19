#pragma once

#include "IWidget.hpp"
#include <algorithm>
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
            range = {0, 0};
        }
        ListView(const NbRect<int>& rect) noexcept : IWidget(rect)
        {
            range = {0, rect.height};
        }

        const char* getClassName() const override
        {
            return CLASS_NAME;
        }

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

        void onMouseWheel(
            const NbPoint<int>& pos,
            int                 delta
        ) override
        {
            if (!rect.isInside(pos))
            {
                return;
            }

            int contentHeight = (int)items.size() * HEIGHT_OF_ITEM_IN_PIXEL;
            int visibleHeight = rect.height;
            int maxScroll     = std::max(0, contentHeight - visibleHeight);

            int scrollAmount = (delta > 0 ? -HEIGHT_OF_ITEM_IN_PIXEL : HEIGHT_OF_ITEM_IN_PIXEL);

            range.first = std::clamp(range.first + scrollAmount, 0, maxScroll);
        }

        void setScrollOffset(int offset)
        {
            int maxScroll =
                std::max(0, (int)(items.size() * HEIGHT_OF_ITEM_IN_PIXEL) - rect.height);
            range.first = std::clamp(offset, 0, maxScroll);
        }

        int getScrollOffset() const
        {
            return range.first;
        }


        std::string getItemText(size_t index) const
        {
            return (index < items.size()) ? items[index] : "";
        }

        size_t getCount() const
        {
            return items.size();
        }
        std::optional<size_t> getSelectedIndex() const
        {
            return selectedIndex;
        }
        int getLastHitIndex() const
        {
            return lastHitIndex;
        }

        bool hitTest(const NbPoint<int>& pos) override
        {
            if (!rect.isInside(pos))
            {
                lastHitIndex = -1;
                return false;
            }
            int absoluteY = (pos.y - rect.y) + range.first;
            lastHitIndex  = absoluteY / HEIGHT_OF_ITEM_IN_PIXEL;

            if (lastHitIndex < 0 || lastHitIndex >= (int)items.size())
            {
                lastHitIndex = -1;
                return false;
            }
            return true;
        }

        bool hitTestClick(const NbPoint<int>& pos) noexcept override
        {
            if (!hitTest(pos))
            {
                return false;
            }

            selectedIndex = lastHitIndex;
            onItemSelectedSignal.emit(lastHitIndex);
            onItemClickSignal.emit(lastHitIndex);
            return true;
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

    public:
        Signal<void(int)> onItemSelectedSignal;
        Signal<void(int)> onItemClickSignal;

    private:
        std::vector<std::string> items;
        std::optional<size_t>    selectedIndex;
        int                      lastHitIndex = -1;
        std::pair<int, int>      range; 
        NbSize<int>              measuredSize;
    };
} // namespace Widgets