#pragma once

#include "Layout/LayoutNode.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/IWidget.hpp"
#include <Core.hpp>
#include <algorithm>
#include <memory>
#include <string>

namespace Widgets
{
    class SectionWidget : public IWidget
    {
    public:
        DECLARE_WIDGET_CLASS_NAME(SectionWidget);

        SectionWidget(
            const std::wstring& title,
            const NbRect<int>& rc
        )
            : headerButton(
                  std::make_unique<Button>(
                                            rc
                  )
              ),
              collapsed(false),
              IWidget({})
        {
            innerLayout = std::make_unique<NNsLayout::VLayout>();
        }

        ~SectionWidget() = default;

        // Для renderSection
        const char* getClassName() const override
        {
            return CLASS_NAME;
        }

        std::wstring getTitle() const
        {
            return headerButton->getText();
        }

        bool isExpanded() const noexcept
        {
            return !collapsed;
        }

        NNsLayout::LayoutNode* getInnerLayout() noexcept
        {
            return innerLayout.get();
        }

        void toggleCollapse()
        {
            collapsed = !collapsed;
            innerLayout->setSpacing(collapsed ? 0 : 5);
            innerLayout->markDirty();
        }

        const NbSize<int>& measure(const NbSize<int>& available) noexcept
        {
            headerButton->measure(available);
            if (!collapsed)
            {
                innerLayout->measure(available);
                const auto& sz = innerLayout->getMeasuredSize();
                measuredSize.width = std::max(headerButton->getMeasuredSize().width, sz.width);
                measuredSize.height = headerButton->getMeasuredSize().height + sz.height;
            }
            else
            {
                measuredSize = headerButton->getMeasuredSize();
            }
            return measuredSize;
        }

        void layout(const NbRect<int>& bounds) noexcept
        {
            NbRect<int> headerRect = bounds;
            headerRect.height = headerButton->getMeasuredSize().height;
            headerButton->setRect(headerRect);
            headerButton->layout(headerRect);

            if (!collapsed)
            {
                NbRect<int> innerRect = bounds;
                innerRect.y += headerRect.height;
                innerRect.height -= headerRect.height;
                innerLayout->setRect(innerRect);
                innerLayout->layout(innerRect);
            }
        }

        void onHeaderClick()
        {
            toggleCollapse();
        }

        // Дочерние виджеты для рендера
        auto& getChildrens() noexcept
        {
            return innerLayout->getChildren();
        }

    private:
        std::unique_ptr<Button> headerButton;
        std::unique_ptr<NNsLayout::VLayout> innerLayout;
        bool collapsed;
    };
} // namespace Widgets