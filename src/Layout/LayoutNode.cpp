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
            int totalFixed = 0;
            float totalRelative = 0.0f;

            for (auto& child : children)
            {
                auto& st = child->style;

                if (st.heightSizeType == SizeType::RELATIVE) {
                    totalRelative += st.height;
                    continue;
                }

                child->measure(available);
                auto s = child->getMeasuredSize();

                int fullH =
                    st.margin.top + st.margin.bottom +
                    st.border.width * 2 +
                    st.padding.top + st.padding.bottom +
                    s.height;

                totalFixed += fullH;
            }

            int remaining = available.height - totalFixed;
            if (remaining < 0) remaining = 0;

            measuredSize.width = available.width;
            measuredSize.height = totalFixed;

            if (totalRelative > 0.0001f)
                measuredSize.height += remaining;
        }



        void VLayout::layout(const NbRect<int>& bounds) noexcept
        {
            layoutRect = bounds;

            float totalRelative = 0.0f;
            int totalFixed = 0;

            for (auto& child : children)
            {
                auto& st = child->style;

                switch (st.heightSizeType)
                {
                case SizeType::RELATIVE:
                    totalRelative += st.height;
                    break;

                case SizeType::ABSOLUTE:
                    totalFixed += static_cast<int>(st.height)
                        + st.margin.top + st.margin.bottom
                        + st.border.width * 2
                        + st.padding.top + st.padding.bottom;
                    break;

                case SizeType::AUTO:
                    totalFixed += child->getMeasuredSize().height
                        + st.margin.top + st.margin.bottom
                        + st.border.width * 2
                        + st.padding.top + st.padding.bottom;
                    break;
                }
            }

            int remaining = bounds.height - totalFixed;
            if (remaining < 0) remaining = 0;

            int y = bounds.y;

            for (auto& child : children)
            {
                auto& st = child->style;
                int contentHeight = 0;

                switch (st.heightSizeType)
                {
                case SizeType::ABSOLUTE:
                    contentHeight = static_cast<int>(st.height);
                    break;

                case SizeType::AUTO:
                    contentHeight = child->getMeasuredSize().height;
                    break;

                case SizeType::RELATIVE:
                    if (totalRelative > 0.0001f)
                        contentHeight = static_cast<int>(remaining * (st.height / totalRelative));
                    else
                        contentHeight = 0;
                    break;
                }

                int fullHeight =
                    st.margin.top +
                    st.border.width +
                    st.padding.top +
                    contentHeight +
                    st.padding.bottom +
                    st.border.width +
                    st.margin.bottom;

                NbRect<int> childRect;
                childRect.x = bounds.x + st.margin.left + st.border.width + st.padding.left;
                childRect.y = y + st.margin.top + st.border.width + st.padding.top;
                childRect.width =
                    bounds.width
                    - (st.margin.left + st.margin.right)
                    - (st.border.width * 2)
                    - (st.padding.left + st.padding.right);
                childRect.height = contentHeight;

                child->setRect(childRect);
                child->layout(childRect);

                y += fullHeight;
            }
        }






        void LayoutWidget::measure(const NbSize<int>& available) noexcept
        {
            const auto natural = widget ? widget->computeContentSize() : NbSize<int>{ 0, 0 };

            int w = 0;
            int h = 0;

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

            //w += style.margin.left + style.margin.right;
            //h += style.margin.top + style.margin.bottom;

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

            //inner.x += style.margin.left;
            //inner.y += style.margin.top;
            //inner.width  -= style.margin.left + style.margin.right;
            //inner.height -= style.margin.top  + style.margin.bottom;

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
