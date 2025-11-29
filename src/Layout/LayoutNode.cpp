#include "LayoutNode.hpp"

#include "Widgets/IWidget.hpp"
#include "WindowInterface/IWindow.hpp"

#undef ABSOLUTE
#undef RELATIVE

namespace NNsLayout
{
    void HLayout::measure(const NbSize<int>& available) noexcept
    {
        int totalFixed = 0;
        int maxHeight = 0;
        int flexCount = 0;

        // 1. Первый проход: собираем информацию
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
        int x = bounds.x;

        for (auto& child : children)
        {
            NbSize<int> s = child->getMeasuredSize();

            // если ширина превышает оставшееся пространство
            int width = (std::min)(s.width, bounds.x + bounds.width - x);
            int height = (std::min)(s.height, bounds.height);

            NbRect<int> childRect{
                x,
                bounds.y,
                width,
                height
            };

            child->layout(childRect);

            x += width;
            if (x >= bounds.x + bounds.width) {
                break; // дальше места нет
            }
        }
    }


    void LayoutWidget::measure(const NbSize<int>& available) noexcept
    {
        const auto natural = widget ? widget->computeContentSize() : NbSize<int>{ 0, 0 };

        int w = 0;
        int h = 0;

        // WIDTH
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

        // HEIGHT
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

        // padding
        w += style.padding.left + style.padding.right;
        h += style.padding.top + style.padding.bottom;

        // border
        w += style.border.width * 2;
        h += style.border.width * 2;

        // margin
        w += style.margin.left + style.margin.right;
        h += style.margin.top + style.margin.bottom;

        // clamp
        w = (std::min)(w, available.width);
        h = (std::min)(h, available.height);

        // min/max
        w = (std::max)(w, style.minSize.width);
        h = (std::max)(h, style.minSize.height);

        if (style.maxSize.width > 0) w = (std::min)(w, style.maxSize.width);
        if (style.maxSize.height > 0) h = (std::min)(h, style.maxSize.height);

        measuredSize = { w, h };
    }

    
    void LayoutWidget::layout(const NbRect<int>& bounds) noexcept
    {
        if (!widget) return;

        NbRect<int> inner = bounds;

        // remove margin
        inner.x += style.margin.left;
        inner.y += style.margin.top;
        inner.width  -= style.margin.left + style.margin.right;
        inner.height -= style.margin.top  + style.margin.bottom;

        // remove border
        inner.x += style.border.width;
        inner.y += style.border.width;
        inner.width  -= style.border.width * 2;
        inner.height -= style.border.width * 2;

        // remove padding
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

        NbRect<int> client = ownerWindow->getClientRect();

        if (!children.empty())
        {
            children[0]->layout(client);
        }
    }
}
