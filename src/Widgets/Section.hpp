#ifndef NBUI_SRC_WIDGETS_SECTION_HPP
#define NBUI_SRC_WIDGETS_SECTION_HPP

#include "IWidget.hpp"
#include <string>

namespace Widgets
{
    class SectionWidget : public IWidget
    {
    public:

        static constexpr const char* CLASS_NAME = "SectionWidget";


        SectionWidget(
            const std::wstring& title,
            const NbRect<int>& rect,
            uint16_t zIndexOrder = 0
        )
            : IWidget(
                  rect,
                  zIndexOrder
              ),
              title(title)
        {
            expanded = true;
            headerHeight = 24; 
            
        }

        virtual const char* getClassName() const override
        {
            return CLASS_NAME;
        }

        void toggle()
        {
            expanded = !expanded;
            for (auto child : childrens)
            {
                child->hide(!expanded);
            }

            isSizeChange = true;
            onSizeChangedSignal.emit(rect);
        }

        // Логика измерения размера (IMeasureLayout)
        const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
        {
            int width = maxSize.width;
            int height = headerHeight;

            if (expanded)
            {
                NbSize<int> childAvailable{maxSize.width, maxSize.height};

                for (auto* child : childrens)
                {
                    if (!child || child->isHide())
                    {
                        continue;
                    }

                    auto childSize = child->measure(childAvailable);
                    height += childSize.height;
                    width = std::max(width, childSize.width);
                }

            }

            measuredSize = {width, height};
            return measuredSize;
        }

        void layout(const NbRect<int>& rect) noexcept override
        {
            setRect(rect);

            if (!expanded)
            {
                for (auto* child : childrens)
                {
                    if (child)
                    {
                        child->layout({0, 0, 0, 0});
                    }
                }

                return;
            }

            int currentY = rect.y + headerHeight;

            NbSize<int> innerAvailable{rect.width, rect.height - headerHeight};

            for (auto* child : childrens)
            {
                if (!child || child->isHide())
                {
                    continue;
                }

                auto childSize = child->measure(innerAvailable);

                NbRect<int> childRect{rect.x, currentY, rect.width, childSize.height};

                child->layout(childRect);

                currentY += childSize.height;
            }

        }

        virtual bool hitTest(const NbPoint<int>& pos) override
        {
            NbRect<int> headerRect = {
                rect.x, 
                rect.y,
                rect.width,
                headerHeight
            };

            if (headerRect.isInside(pos))
            {
                return true;
            }

            return false;
        }

        virtual bool hitTestClick(const NbPoint<int>& pos) noexcept override
        {
            NbRect<int> headerRect = {
                rect.x, 
                rect.y,
                rect.width,
                headerHeight
            };

            if (headerRect.isInside(pos))
            {
                toggle();
                return true;
            }

            IWidget::hitTestClick(pos);
        }

        bool isExpanded() const
        {
            return expanded;
        }
        void setExpanded(bool expand)
        {
            if (expanded != expand)
            {
                toggle();
            }
        }

        const std::wstring& getTitle() const noexcept
        {
            return title;
        }

    protected:
        std::wstring title;
        bool expanded;
        int headerHeight;
        NbSize<int> measuredSize; 
    };
}; 

#endif