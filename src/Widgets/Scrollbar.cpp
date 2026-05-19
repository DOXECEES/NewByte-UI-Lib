#include "Scrollbar.hpp"
#include <algorithm>

namespace Widgets
{
    ScrollArea::ScrollArea(
        const NbRect<int>& rect,
        const uint16_t zIndexOrder
    )
        : IWidget(
              rect,
              zIndexOrder
          )
    {
        setSizePolicy(SizePolicy::EXPANDING, SizePolicy::EXPANDING);
    }

    // Берем первый дочерний виджет как контент для прокрутки
    IWidget* ScrollArea::getContentWidget() const noexcept
    {
        return childrens.empty() ? nullptr : childrens.front().get();
    }

    const NbSize<int>& ScrollArea::measure(const NbSize<int>& maxSize) noexcept
    {
        if (auto content = getContentWidget())
        {
            // Даем контенту бесконечную высоту для вычисления реального размера,
            // но ограничиваем ширину, оставляя место под скроллбар
            NbSize<int> contentMaxSize = {maxSize.width - scrollbarWidth, 9999999};
            content->measure(contentMaxSize);
        }

        measuredSize = maxSize;
        return measuredSize;
    }

    void ScrollArea::layout(const NbRect<int>& layoutRect) noexcept
    {
        setRect(layoutRect);

        if (auto content = getContentWidget())
        {
            NbSize<int> contentSize = content->getMeasuredSize();

            // Вычисляем максимум прокрутки
            maxScrollY = std::max(0, contentSize.height - layoutRect.height);
            clampScroll();

            // Располагаем контент, сдвигая его вверх на -scrollY
            NbRect<int> contentLayoutRect = {
                layoutRect.x, layoutRect.y - static_cast<int>(scrollY),
                layoutRect.width - scrollbarWidth, contentSize.height
            };
            content->layout(contentLayoutRect);
        }
    }

    bool ScrollArea::hitTest(const NbPoint<int>& pos)
    {
        return pos.x >= rect.x && pos.x <= rect.x + rect.width && pos.y >= rect.y &&
               pos.y <= rect.y + rect.height;
    }

    bool ScrollArea::hitTestClick(const NbPoint<int>& pos) noexcept
    {
        if (!hitTest(pos))
        {
            return false;
        }

        // Перехватываем клик по скроллбару
        if (canScroll())
        {
            NbRect<int> track = getScrollbarTrackRect();
            if (pos.x >= track.x && pos.x <= track.x + track.width && pos.y >= track.y &&
                pos.y <= track.y + track.height)
            {
                NbRect<int> thumb = getThumbRect();
                if (pos.y >= thumb.y && pos.y <= thumb.y + thumb.height)
                {
                    isDraggingThumb = true;
                    dragStartMouseY = pos.y;
                    dragStartScrollY = scrollY;
                }
                else
                {
                    // Клик мимо ползунка - прыгаем
                    float ratio = static_cast<float>(pos.y - track.y) / track.height;
                    scrollY = ratio * maxScrollY;
                    clampScroll();
                    updateContentPosition();
                }
                return true;
            }
        }

        // Если клик не по скроллбару, отдаем детям
        return IWidget::hitTestClick(pos);
    }

    void ScrollArea::onMouseMove(const MouseState& mouseState) noexcept
    {
        if (isDraggingThumb)
        {
            int deltaY = mouseState.position.y - dragStartMouseY;
            NbRect<int> track = getScrollbarTrackRect();
            NbRect<int> thumb = getThumbRect();

            int maxThumbMove = track.height - thumb.height;
            if (maxThumbMove > 0)
            {
                float scrollRatio = static_cast<float>(deltaY) / maxThumbMove;
                scrollY = dragStartScrollY + (scrollRatio * maxScrollY);
                clampScroll();
                updateContentPosition();
            }
        }

        IWidget::onMouseMove(mouseState);
    }

    void ScrollArea::onRelease() noexcept
    {
        isDraggingThumb = false;
        IWidget::onRelease();
    }

    void ScrollArea::onMouseWheel(float delta) noexcept
    {
        if (!canScroll())
        {
            return;
        }

        scrollY -= delta * scrollSpeed;
        clampScroll();
        updateContentPosition();
    }

    void ScrollArea::clampScroll() noexcept
    {
        if (scrollY < 0.0f)
        {
            scrollY = 0.0f;
        }
        if (scrollY > maxScrollY)
        {
            scrollY = static_cast<float>(maxScrollY);
        }
    }

    void ScrollArea::updateContentPosition() noexcept
    {
        if (auto content = getContentWidget())
        {
            NbSize<int> contentSize = content->getMeasuredSize();
            NbRect<int> contentLayoutRect = {
                rect.x, rect.y - static_cast<int>(scrollY), rect.width - scrollbarWidth,
                contentSize.height
            };
            content->layout(contentLayoutRect);
        }
    }

    NbRect<int> ScrollArea::getScrollbarTrackRect() const noexcept
    {
        return {rect.x + rect.width - scrollbarWidth, rect.y, scrollbarWidth, rect.height};
    }

    NbRect<int> ScrollArea::getThumbRect() const noexcept
    {
        NbRect<int> track = getScrollbarTrackRect();
        auto content = getContentWidget();

        if (!canScroll() || !content)
        {
            return {track.x, track.y, track.width, 0};
        }

        int contentHeight = content->getMeasuredSize().height;
        if (contentHeight <= 0)
        {
            return {track.x, track.y, track.width, 0};
        }

        float visibleRatio = static_cast<float>(rect.height) / contentHeight;
        int thumbHeight = std::max(20, static_cast<int>(track.height * visibleRatio));

        float scrollRatio = scrollY / maxScrollY;
        int thumbY = track.y + static_cast<int>(scrollRatio * (track.height - thumbHeight));

        return {track.x, thumbY, track.width, thumbHeight};
    }
} // namespace Widgets