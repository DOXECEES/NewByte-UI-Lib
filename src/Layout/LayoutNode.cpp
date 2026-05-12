// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "LayoutNode.hpp"

#include "Widgets/Button.hpp"
#include "Widgets/ButtonGroup.hpp"
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

            int fullW = s.width + style.margin.left + style.margin.right + 
                        style.border.width.left + style.border.width.right + 
                        style.padding.left + style.padding.right;

            int fullH = s.height + style.margin.top + style.margin.bottom + 
                        style.border.width.top + style.border.width.bottom + 
                        style.padding.top + style.padding.bottom;

            totalFixed += fullW;
            maxHeight = (std::max)(maxHeight, fullH);
        }

        measuredSize = {totalFixed, maxHeight};
    }

    void HLayout::layout(const NbRect<int>& bounds) noexcept
    {
        layoutRect = bounds;
        int x = bounds.x;

        float totalRelative = 0.0f;
        int totalFixed = 0;

        for (auto& child : children)
        {
            const auto& st = child->style;
            int decorationW = st.margin.left + st.margin.right + st.border.width.left + st.border.width.right + st.padding.left + st.padding.right;

            switch (st.widthSizeType)
            {
            case SizeType::RELATIVE:
                totalRelative += st.width;
                totalFixed += decorationW;
                break;
            case SizeType::ABSOLUTE:
                totalFixed += static_cast<int>(st.width) + decorationW;
                break;
            case SizeType::AUTO:
                totalFixed += child->getMeasuredSize().width + decorationW;
                break;
            }
        }

        int remainingWidth = (std::max)(0, bounds.width - totalFixed);


        for (auto& child : children)
        {
            const auto& st    = child->style;
            int         width = 0;

            switch (st.widthSizeType)
            {
            case SizeType::ABSOLUTE:
                width = static_cast<int>(st.width);
                break;
            case SizeType::RELATIVE:
                width = (totalRelative > 0)
                            ? static_cast<int>(remainingWidth * (st.width / totalRelative))
                            : 0;
                break;
            case SizeType::AUTO:
                width = child->getMeasuredSize().width;
                break;
            }

            int decorationW   = st.margin.left + st.margin.right + st.border.width.left +
                                st.border.width.right + st.padding.left + st.padding.right;
            int maxAvailableW = (std::max)(0, (bounds.x + bounds.width) - (x + decorationW));
            width             = (std::min)(width, maxAvailableW);

            int decorationH = st.margin.top + st.margin.bottom + st.border.width.top +
                              st.border.width.bottom + st.padding.top + st.padding.bottom;
            int height =
                (std::max)(0, bounds.height - decorationH); 

            if (st.heightSizeType == SizeType::ABSOLUTE)
            {
                height = (std::min)(height, static_cast<int>(st.height)); 
            }
            else if (st.heightSizeType == SizeType::AUTO)
            {
                height = (std::min)(height, child->getMeasuredSize().height); 
            }

            NbRect<int> childRect;
            childRect.x      = x + st.margin.left + st.border.width.left + st.padding.left;
            childRect.y      = bounds.y + st.margin.top + st.border.width.top + st.padding.top;
            childRect.width  = (std::max)(0, width);
            childRect.height = (std::max)(0, height);

            child->setRect(childRect);
            child->layout(childRect);

            x += width + decorationW;
        }

    }

    void VLayout::measure(const NbSize<int>& available) noexcept
    {
        int   totalFixed    = 0;
        float totalRelative = 0.0f;
        int   visibleCount  = 0;

        for (auto& child : children)
        {
            auto& st = child->style;
            visibleCount++;

            if (st.heightSizeType == SizeType::RELATIVE)
            {
                totalRelative += st.height;
                continue;
            }

            int decorationW = st.margin.left + st.margin.right + st.border.width.left +
                              st.border.width.right + st.padding.left + st.padding.right;

            int childInnerWidth = (std::max)(0, available.width - decorationW);

            child->measure({childInnerWidth, available.height});

            auto s = child->getMeasuredSize();

            int decorationH = st.margin.top + st.margin.bottom + st.border.width.top +
                              st.border.width.bottom + st.padding.top + st.padding.bottom;

            if (st.heightSizeType == SizeType::ABSOLUTE)
            {
                totalFixed += static_cast<int>(st.height) + decorationH;
            }
            else
            { 
                totalFixed += s.height + decorationH;
            }
        }

        if (visibleCount > 1)
        {
            totalFixed += (visibleCount - 1) * spacing;
        }

        measuredSize.width = available.width;

        if (totalRelative > 0.0001f)
        {
            measuredSize.height = (std::max)(totalFixed, available.height);
        }
        else
        {
            measuredSize.height = totalFixed;
        }
    }

    void VLayout::layout(const NbRect<int>& bounds) noexcept
    {
        layoutRect = bounds;

        float totalRelative              = 0.0f;
        int   fixedHeightWithDecorations = 0;
        int   visibleCount               = (int)children.size();

        for (auto& child : children)
        {
            auto& st          = child->style;
            int   decorationH = st.margin.top + st.margin.bottom + st.border.width.top +
                                st.border.width.bottom + st.padding.top + st.padding.bottom;

            if (st.heightSizeType == SizeType::RELATIVE)
            {
                totalRelative += st.height;
            }
            else if (st.heightSizeType == SizeType::ABSOLUTE)
            {
                fixedHeightWithDecorations += static_cast<int>(st.height) + decorationH;
            }
            else
            { 
                fixedHeightWithDecorations += child->getMeasuredSize().height + decorationH;
            }
        }

        if (visibleCount > 1)
        {
            fixedHeightWithDecorations += (visibleCount - 1) * spacing;
        }

        int remaining = (std::max)(0, bounds.height - fixedHeightWithDecorations);
        int y         = bounds.y - scrollOffset;

        for (size_t i = 0; i < children.size(); ++i)
        {
            auto& child = children[i];
            auto& st    = child->style;

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

            int decorationW = st.margin.left + st.margin.right + st.border.width.left +
                              st.border.width.right + st.padding.left + st.padding.right;

            NbRect<int> childRect;
            childRect.x      = bounds.x + st.margin.left + st.border.width.left + st.padding.left;
            childRect.y      = y + st.margin.top + st.border.width.top + st.padding.top;
            childRect.width  = (std::max)(0, bounds.width - decorationW);
            childRect.height = contentHeight;

            child->setRect(childRect);
            child->layout(childRect);

            y += contentHeight + st.margin.top + st.margin.bottom + st.border.width.top +
                 st.border.width.bottom + st.padding.top + st.padding.bottom;

            if (i < children.size() - 1)
            {
                y += spacing;
            }
        }
    }


    void VLayout::setSpacing(int value) noexcept
    {
        spacing = value;
    }

    

    

    GridLayout::GridLayout(int cols) noexcept
        : LayoutNode(nullptr),
          columns(cols)
    {
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

        measuredSize.width = maxChildW * columns;
        measuredSize.height = maxChildH * numRows;

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

        int totalChildren = static_cast<int>(children.size());
        int numRows = (rows > 0) ? rows : (totalChildren + columns - 1) / columns;

        float cellW = static_cast<float>(bounds.width) / columns;
        float cellH = static_cast<float>(bounds.height) / numRows;

        int index = 0;
        for (auto& child : children)
        {
            int col = index % columns;
            int row = index / columns;

            const auto& st = child->style;

            int cellX = bounds.x + static_cast<int>(col * cellW);
            int cellY = bounds.y + static_cast<int>(row * cellH);
            int currentCellW = static_cast<int>(cellW);
            int currentCellH = static_cast<int>(cellH);

            NbRect<int> childRect;
            childRect.x = cellX + st.margin.left;
            childRect.y = cellY + st.margin.top;

            childRect.width = currentCellW - (st.margin.left + st.margin.right);
            childRect.height = currentCellH - (st.margin.top + st.margin.bottom);

            if (st.widthSizeType == SizeType::ABSOLUTE)
            {
                childRect.width = (std::min)(childRect.width, static_cast<int>(st.width));
            }
            if (st.heightSizeType == SizeType::ABSOLUTE)
            {
                childRect.height = (std::min)(childRect.height, static_cast<int>(st.height));
            }

            child->setRect(childRect);
            child->layout(childRect);

            index++;
            if (rows > 0 && index >= columns * rows)
            {
                break;
            }
        }
    }

    

    FlowLayout::FlowLayout() noexcept : LayoutNode(nullptr)
    {
    }

    void FlowLayout::measure(const NbSize<int>& available) noexcept
    {
        if (children.empty())
        {
            measuredSize = {0, 0};
            return;
        }

        auto& firstStyle = children[0]->style;

        int itemW =
            static_cast<int>(firstStyle.width) + firstStyle.margin.left + firstStyle.margin.right;
        int itemH =
            static_cast<int>(firstStyle.height) + firstStyle.margin.top + firstStyle.margin.bottom;

        int columns = available.width / itemW;
        if (columns < 1)
        {
            columns = 1;
        }

        int totalItems = static_cast<int>(children.size());
        int rows = (totalItems + columns - 1) / columns;

        measuredSize.width = available.width;
        measuredSize.height = rows * itemH;

        measuredSize.height = (std::max)(measuredSize.height, style.minSize.height);
        if (style.maxSize.height > 0)
        {
            measuredSize.height = (std::min)(measuredSize.height, style.maxSize.height);
        }
    }

    void FlowLayout::layout(const NbRect<int>& bounds) noexcept
    {
        layoutRect = bounds;
        if (children.empty())
        {
            return;
        }

        auto& firstStyle = children[0]->style;
        int childW = static_cast<int>(firstStyle.width);
        int childH = static_cast<int>(firstStyle.height);

        int stepX = childW + firstStyle.margin.left + firstStyle.margin.right;
        int stepY = childH + firstStyle.margin.top + firstStyle.margin.bottom;

        int columns = bounds.width / stepX;
        if (columns < 1)
        {
            columns = 1;
        }

        int totalGridWidth = columns * stepX;
        int offsetX = (bounds.width - totalGridWidth) / 2;

        int index = 0;
        for (auto& child : children)
        {
            int col = index % columns;
            int row = index / columns;

            auto& st = child->style;

            int x = bounds.x + offsetX + (col * stepX) + st.margin.left;

            int y = bounds.y + (row * stepY) + st.margin.top;

            NbRect<int> childRect{x, y, childW, childH};

            child->setRect(childRect);
            child->layout(childRect);

            index++;
        }
    }

    LayoutNode::LayoutNode(Widgets::IWidget* owner) noexcept : ownerWidget(owner)
    {
    }
    void LayoutNode::clearChilds() noexcept
    {
        children.clear();
    }
    void LayoutNode::markDirty() noexcept
    {
        if (dirty)
        {
            return;
        }
        dirty = true;
        if (parent)
        {
            parent->markDirty();
        }
    }
}; 
