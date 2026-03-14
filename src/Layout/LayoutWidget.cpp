#include "LayoutWidget.hpp"

#include "Alghorithm.hpp"
#include "Widgets/IWidget.hpp"
namespace NNsLayout
{
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

    LayoutWidget::LayoutWidget(std::shared_ptr<Widgets::IWidget> w) noexcept
        : LayoutNode(w.get()),
          widget(std::move(w))
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

    std::shared_ptr<Widgets::IWidget> LayoutWidget::getWidget() const noexcept
    {
        return widget;
    }



        void LayoutWidget::measure(const NbSize<int>& available) noexcept
        {
            const auto natural = widget ? widget->measure(available) : NbSize<int>{0, 0};

            int childrenWidth = 0;
            int childrenHeight = 0;

            for (auto& child : children)
            {
                child->measure(available);
                NbSize<int> childSize = child->getMeasuredSize();
                childrenWidth += childSize.width;
                childrenHeight = (nbstl::max)(childrenHeight, childSize.height);
            }

            int internalW = style.padding.left + style.padding.right + style.border.width.left +
                            style.border.width.right;
            int internalH = style.padding.top + style.padding.bottom + style.border.width.top +
                            style.border.width.bottom;

            int w = 0;
            switch (style.widthSizeType)
            {
            case SizeType::ABSOLUTE:
                w = static_cast<int>(style.width);
                break;
            case SizeType::RELATIVE:
                w = static_cast<int>(available.width * style.width);
                break;
            case SizeType::AUTO:
                w = (!children.empty()) ? childrenWidth : natural.width;
                break;
            case SizeType::FLEX:
                w = available.width - internalW; 
                break;
            }

            int h = 0;
            switch (style.heightSizeType)
            {
            case SizeType::ABSOLUTE:
                h = static_cast<int>(style.height);
                break;
            case SizeType::RELATIVE:
                h = static_cast<int>(available.height * style.height);
                break;
            case SizeType::AUTO:
                h = (!children.empty()) ? childrenHeight : natural.height;
                break;
            case SizeType::FLEX:
                h = available.height - internalH;
                break;
            }

            w += internalW;
            h += internalH;

            measuredSize = {
                (nbstl::max)(style.minSize.width, w), (nbstl::max)(style.minSize.height, h)
            };
        }

        void LayoutWidget::layout(const NbRect<int>& bounds) noexcept
        {
            if (!widget)
            {
                return;
            }

            NbRect<int> inner = bounds;

            int borderPaddingW = style.border.width.left + style.border.width.right +
                                 style.padding.left + style.padding.right;
            int borderPaddingH = style.border.width.top + style.border.width.bottom +
                                 style.padding.top +
                                 style.padding.bottom;

            inner.x += style.border.width.left + style.padding.left;
            inner.y += style.border.width.top + style.padding.top;

            inner.width -= borderPaddingW;
            inner.height -= borderPaddingH;

            if (inner.width < 0)
            {
                inner.width = 0;
            }
            if (inner.height < 0)
            {
                inner.height = 0;
            }

            widget->layout(inner);

            if (!children.empty())
            {
                int currentX = inner.x;

                for (auto& child : children)
                {
                    NbSize<int> childSize = child->getMeasuredSize();

                    int finalChildWidth = childSize.width;
                    if (child->style.widthSizeType == SizeType::RELATIVE)
                    {
                        finalChildWidth = static_cast<int>(inner.width * child->style.width);
                    }

                    NbRect<int> childRect = {currentX, inner.y, finalChildWidth, inner.height};

                    child->layout(childRect);

                    currentX += finalChildWidth;
                }
            }
        }
}; 