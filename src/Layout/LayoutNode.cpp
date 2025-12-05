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
            switch (child->style.widthSizeType)
            {
            case SizeType::RELATIVE: totalRelative += child->style.width; break;
            case SizeType::ABSOLUTE: totalFixed += static_cast<int>(child->style.width); break;
            case SizeType::AUTO: totalFixed += child->getMeasuredSize().width; break;
            }
        }

        int remainingWidth = bounds.width - totalFixed;

        for (auto& child : children)
        {
            int width = 0;

            switch (child->style.widthSizeType)
            {
            case SizeType::ABSOLUTE: width = static_cast<int>(child->style.width); break;
            case SizeType::RELATIVE: width = static_cast<int>(remainingWidth * (child->style.width / totalRelative)); break;
            case SizeType::AUTO: width = child->getMeasuredSize().width; break;
            }

            int height = bounds.height;
            switch (child->style.heightSizeType)
            {
            case SizeType::ABSOLUTE: height = static_cast<int>(child->style.height); break;
            case SizeType::RELATIVE: height = static_cast<int>(bounds.height * child->style.height); break;
            case SizeType::AUTO: height = child->getMeasuredSize().height; break;
            }

            NbRect<int> childRect{ x, bounds.y, width, height };
            child->setRect(childRect);
            child->layout(childRect);

            x += width;
        }
    }


    void VLayout::measure(const NbSize<int>& available) noexcept
    {
        int totalFixedHeight = 0;
        int maxWidth = 0;
        int flexCount = 0;

        for (auto& child : children)
        {
            auto& style = child->style;

            if (style.heightSizeType == SizeType::FLEX)
            {
                flexCount++;
                continue;
            }

            child->measure(available);
            auto s = child->getMeasuredSize();

            totalFixedHeight += s.height + style.margin.top + style.margin.bottom;
            maxWidth = nbstl::max(maxWidth, static_cast<int>(s.width + style.margin.left + style.margin.right));
        }

        measuredSize.width = maxWidth;
        measuredSize.height = totalFixedHeight;

        if (flexCount > 0)
        {
            int remaining = available.height - totalFixedHeight;
            measuredSize.height += remaining; 
        }
    }


    void VLayout::layout(const NbRect<int>& bounds) noexcept
    {
        layoutRect = bounds;
        int y = bounds.y;

        float totalRelative = 0.0f;
        int totalFixed = 0;

        for (auto& child : children)
        {
            switch (child->style.heightSizeType)
            {
            case SizeType::RELATIVE: totalRelative += child->style.height; break;
            case SizeType::ABSOLUTE: totalFixed += static_cast<int>(child->style.height); break;
            case SizeType::AUTO: totalFixed += child->getMeasuredSize().height; break;
            }
        }

        int remainingHeight = bounds.height - totalFixed;

        for (auto& child : children)
        {
            int height = 0;

            switch (child->style.heightSizeType)
            {
            case SizeType::ABSOLUTE: height = static_cast<int>(child->style.height); break;
            case SizeType::RELATIVE: height = static_cast<int>(remainingHeight * (child->style.height / totalRelative)); break;
            case SizeType::AUTO: height = child->getMeasuredSize().height; break;
            }

            int width = bounds.width;
            switch (child->style.widthSizeType)
            {
            case SizeType::ABSOLUTE: width = static_cast<int>(child->style.width); break;
            case SizeType::RELATIVE: width = static_cast<int>(bounds.width * child->style.width); break;
            case SizeType::AUTO: width = child->getMeasuredSize().width; break;
            }

            NbRect<int> childRect{ bounds.x, y, width, height };
            child->setRect(childRect);
            child->layout(childRect);

            y += height;
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

        w += style.padding.left + style.padding.right;
        h += style.padding.top + style.padding.bottom;

        w += style.border.width * 2;
        h += style.border.width * 2;

        w += style.margin.left + style.margin.right;
        h += style.margin.top + style.margin.bottom;

        w = (nbstl::min)(w, available.width);
        h = (nbstl::min)(h, available.height);

        w = (nbstl::max)(w, style.minSize.width);
        h = (nbstl::max)(h, style.minSize.height);

        if (style.maxSize.width > 0) w = (nbstl::min)(w, style.maxSize.width);
        if (style.maxSize.height > 0) h = (nbstl::min)(h, style.maxSize.height);

        measuredSize = { w, h };
    }

    
    void LayoutWidget::layout(const NbRect<int>& bounds) noexcept
    {
        if (!widget) return;

        NbRect<int> inner = bounds;

        inner.x += style.margin.left;
        inner.y += style.margin.top;
        inner.width  -= style.margin.left + style.margin.right;
        inner.height -= style.margin.top  + style.margin.bottom;

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

        NbRect<int> client = ownerWindow->getClientRect();

        if (!children.empty())
        {
            children[0]->layout(client);
        }
    }
}
