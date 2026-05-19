#ifndef NBUI_SRC_WIDGETS_SCROLLAREA_HPP
#define NBUI_SRC_WIDGETS_SCROLLAREA_HPP

#include "IWidget.hpp"

namespace Widgets
{
    class ScrollArea : public IWidget
    {
    public:
        DECLARE_WIDGET_CLASS_NAME(ScrollArea);

        ScrollArea(
            const NbRect<int>& rect =
                {0,
                 0,
                 0,
                 0},
            const uint16_t zIndexOrder = 0
        );
        virtual ~ScrollArea() = default;

        // --- IMeasureLayout ---
        virtual const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override;
        virtual void layout(const NbRect<int>& layoutRect) noexcept override;

        // --- IWidget ---
        virtual const char* getClassName() const override
        {
            return CLASS_NAME;
        }

        virtual bool hitTest(const NbPoint<int>& pos) override;
        virtual bool hitTestClick(const NbPoint<int>& pos) noexcept override;

        virtual void onMouseMove(const MouseState& mouseState) noexcept override;
        virtual void onRelease() noexcept override;

        // Метод прокрутки (нужно будет вызывать из обработчика событий окна)
        virtual void onMouseWheel(float delta) noexcept;

        NbRect<int> getScrollbarTrackRect() const noexcept;
        NbRect<int> getThumbRect() const noexcept;
        bool canScroll() const noexcept
        {
            return maxScrollY > 0;
        }

    private:
        IWidget* getContentWidget() const noexcept;
        void clampScroll() noexcept;
        void updateContentPosition() noexcept;

        float scrollY = 0.0f;
        int maxScrollY = 0;

        bool isDraggingThumb = false;
        int dragStartMouseY = 0;
        float dragStartScrollY = 0.0f;

        int scrollbarWidth = 12; // Ширина скроллбара
        int scrollSpeed = 30;    // Скорость колесика
    };
} // namespace Widgets

#endif // NBUI_SRC_WIDGETS_SCROLLAREA_HPP