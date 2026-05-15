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
        // available считается как доступный размер под MARGIN-BOX

        // --- margin ---
        const int marginW = style.margin.left + style.margin.right;
        const int marginH = style.margin.top + style.margin.bottom;

        // --- border + padding ---
        const int borderPaddingW = style.border.width.left + style.border.width.right +
                                   style.padding.left + style.padding.right;

        const int borderPaddingH = style.border.width.top + style.border.width.bottom +
                                   style.padding.top + style.padding.bottom;

        // Размер доступный под BORDER-BOX (то есть без margin)
        NbSize<int> availableBox = {
            (nbstl::max)(0, available.width - marginW), (nbstl::max)(0, available.height - marginH)
        };

        // Размер доступный под CONTENT-BOX (то есть без margin/border/padding)
        NbSize<int> availableContent = {
            (nbstl::max)(0, availableBox.width - borderPaddingW),
            (nbstl::max)(0, availableBox.height - borderPaddingH)
        };

        // --- natural size от внутреннего widget ---
        const auto natural = widget ? widget->measure(availableContent) : NbSize<int>{0, 0};

        // --- measure children ---
        int childrenWidth  = 0;
        int childrenHeight = 0;

        for (auto& child : children)
        {
            if (!child)
            {
                continue;
            }

            child->measure(availableContent);

            NbSize<int> childSize = child->getMeasuredSize();
            childrenWidth += childSize.width;
            childrenHeight = (nbstl::max)(childrenHeight, childSize.height);
        }

        // --- вычисляем content size ---
        int contentW = 0;
        switch (style.widthSizeType)
        {
        case SizeType::ABSOLUTE:
            contentW = static_cast<int>(style.width);
            break;

        case SizeType::RELATIVE:
            contentW = static_cast<int>(availableContent.width * style.width);
            break;

        case SizeType::AUTO:
            contentW = (!children.empty()) ? childrenWidth : natural.width;
            break;

        case SizeType::FLEX:
            contentW = availableContent.width;
            break;
        }

        int contentH = 0;
        switch (style.heightSizeType)
        {
        case SizeType::ABSOLUTE:
            contentH = static_cast<int>(style.height);
            break;

        case SizeType::RELATIVE:
            contentH = static_cast<int>(availableContent.height * style.height);
            break;

        case SizeType::AUTO:
            contentH = (!children.empty()) ? childrenHeight : natural.height;
            break;

        case SizeType::FLEX:
            contentH = availableContent.height;
            break;
        }

        // clamp content
        if (contentW < 0)
        {
            contentW = 0;
        }
        if (contentH < 0)
        {
            contentH = 0;
        }

        // --- border-box size ---
        int boxW = contentW + borderPaddingW;
        int boxH = contentH + borderPaddingH;

        // --- применяем minSize (minSize считаем как минимум для BORDER-BOX) ---
        boxW = (nbstl::max)(style.minSize.width, boxW);
        boxH = (nbstl::max)(style.minSize.height, boxH);

        // --- outer size (margin-box) ---
        measuredSize = {boxW + marginW, boxH + marginH};
    }

    void LayoutWidget::layout(const NbRect<int>& bounds) noexcept
    {
        // bounds приходит как MARGIN-BOX (внешняя область)

        if (!widget && children.empty())
        {
            return;
        }

        // --- margin ---
        NbRect<int> box = bounds;

        box.x += style.margin.left;
        box.y += style.margin.top;
        box.width -= (style.margin.left + style.margin.right);
        box.height -= (style.margin.top + style.margin.bottom);

        if (box.width < 0)
        {
            box.width = 0;
        }
        if (box.height < 0)
        {
            box.height = 0;
        }

        // --- border + padding ---
        const int borderPaddingW = style.border.width.left + style.border.width.right;

        const int borderPaddingH = style.border.width.top + style.border.width.bottom;

        NbRect<int> content = box;

        content.x += style.border.width.left;
        content.y += style.border.width.top;

        content.width -= borderPaddingW;
        content.height -= borderPaddingH;

        if (content.width < 0)
        {
            content.width = 0;
        }
        if (content.height < 0)
        {
            content.height = 0;
        }

        // layout внутреннего widget
        if (widget)
        {
            widget->layout(content);
        }

        // layout детей (простая горизонтальная раскладка)
        if (!children.empty())
        {
            int currentX = content.x;

            for (auto& child : children)
            {
                if (!child)
                {
                    continue;
                }

                NbSize<int> childSize = child->getMeasuredSize();

                int finalChildWidth = childSize.width;

                // RELATIVE должен быть от content.width (одна база с measure)
                if (child->style.widthSizeType == SizeType::RELATIVE)
                {
                    finalChildWidth = static_cast<int>(content.width * child->style.width);
                }

                // FLEX растягиваем в оставшееся место
                if (child->style.widthSizeType == SizeType::FLEX)
                {
                    finalChildWidth = (content.x + content.width) - currentX;
                }

                if (finalChildWidth < 0)
                {
                    finalChildWidth = 0;
                }

                // height: если RELATIVE, считаем от content.height
                int finalChildHeight = childSize.height;

                if (child->style.heightSizeType == SizeType::RELATIVE)
                {
                    finalChildHeight = static_cast<int>(content.height * child->style.height);
                }

                if (child->style.heightSizeType == SizeType::FLEX)
                {
                    finalChildHeight = content.height;
                }

                if (finalChildHeight < 0)
                {
                    finalChildHeight = 0;
                }

                NbRect<int> childRect = {currentX, content.y, finalChildWidth, finalChildHeight};

                child->layout(childRect);

                currentX += finalChildWidth;

                // чтобы не убегать за content
                if (currentX > content.x + content.width)
                {
                    break;
                }
            }
        }
    }
}; 