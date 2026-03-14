#include "ButtonGroup.hpp"

#include "Widgets/ButtonGroup.hpp"
#include "Widgets/Button.hpp"

namespace NNsLayout 
{
    ButtonGroup::ButtonGroup(ButtonGroupType type, bool allowNone) noexcept
        : LayoutWidget(std::make_shared<ButtonGroupWidget>())
        , type(type)
        , allowNoneSelected(allowNone)
    {
    }

    void ButtonGroup::measure(const NbSize<int>& available) noexcept
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

        measuredSize = {totalFixed, maxHeight};
    }

    void ButtonGroup::layout(const NbRect<int>& bounds) noexcept
    {
        layoutRect = bounds;
        int x = bounds.x;

        float totalRelative = 0.0f;
        int totalFixed = 0;

        for (auto& child : children)
        {
            const NNsLayout::LayoutStyle& style = child->style;
            switch (style.widthSizeType)
            {
            case SizeType::RELATIVE:
            {
                totalRelative += style.width;
                break;
            }
            case SizeType::ABSOLUTE:
            {
                totalFixed += static_cast<int>(style.width);
                break;
            }
            case SizeType::AUTO:
            {
                totalFixed += child->getMeasuredSize().width;
                break;
            }
            }
        }

        int remainingWidth = bounds.width - totalFixed;

        for (auto& child : children)
        {
            int width = 0;
            const NNsLayout::LayoutStyle& style = child->style;

            switch (style.widthSizeType)
            {
            case SizeType::ABSOLUTE:
            {
                width = static_cast<int>(style.width);
                break;
            }
            case SizeType::RELATIVE:
            {
                width = static_cast<int>(remainingWidth * (style.width / totalRelative));
                break;
            }
            case SizeType::AUTO:
            {
                width = child->getMeasuredSize().width;
                break;
            }
            }

            int height = bounds.height;
            switch (style.heightSizeType)
            {
            case SizeType::ABSOLUTE:
            {
                height = static_cast<int>(style.height);
                break;
            }
            case SizeType::RELATIVE:
            {
                height = static_cast<int>(bounds.height * style.height);
                break;
            }
            case SizeType::AUTO:
            {
                height = child->getMeasuredSize().height;
                break;
            }
            }

            NbRect<int> childRect;
            childRect.y = bounds.y + style.margin.top;
            childRect.height = bounds.height - (style.margin.top + style.margin.bottom);

            childRect.x = x + style.margin.left;
            childRect.width = width;

            child->setRect(childRect);
            child->layout(childRect);

            x += width + style.margin.left + style.margin.right;
        }
    }

    void ButtonGroup::syncInternalState()
    {
        for (auto& child : children)
        {
            if (auto* widgetNode = dynamic_cast<LayoutWidget*>(child.get()))
            {
                auto widget = widgetNode->getWidget();

                widget->onClickCallback = [this, widgetNode]()
                {
                    this->handleSelection(widgetNode);
                };
            }
        }
    }

    void ButtonGroup::handleSelection(LayoutWidget* selectedNode)
    {
        if (type == ButtonGroupType::MULTI)
        {
            if (!selectedNode)
            {
                return;
            }
            auto sharedWidget = selectedNode->getWidget();
            if (!sharedWidget)
            {
                return;
            }

            if (auto* btn = dynamic_cast<Widgets::Button*>(sharedWidget.get()))
            {
                btn->toggleIsChecked();
            }

        }
        else
        {
            for (auto& child : children)
            {
                auto* node = dynamic_cast<LayoutWidget*>(child.get());
                if (!node)
                {
                    continue;
                }

                auto sharedWidget = node->getWidget();
                if (!sharedWidget)
                {
                    continue;
                }

                auto* btn = dynamic_cast<Widgets::Button*>(sharedWidget.get());
                if (!btn)
                {
                    continue;
                }

                bool isTarget = (node == selectedNode);

                if (isTarget && btn->getIsChecked() && !allowNoneSelected)
                {
                    continue;
                }

                btn->setIsChecked(isTarget);
            }
        }
        
        markDirty();
    }

    void ButtonGroup::setCheckedIndex(
        bool state,
        int index
    ) noexcept
    {
        // 1. Проверка границ
        if (index < 0 || static_cast<size_t>(index) >= children.size())
        {
            return;
        }

        // Вспомогательная функция для извлечения кнопки из узла лейаута
        auto getButtonFromNode = [](LayoutNode* node) -> Widgets::Button*
        {
            if (auto* lWidget = dynamic_cast<LayoutWidget*>(node))
            {
                return dynamic_cast<Widgets::Button*>(lWidget->getWidget().get());
            }
            return nullptr;
        };

        // 2. Получаем целевую кнопку
        auto* targetButton = getButtonFromNode(children[index].get());
        if (!targetButton)
        {
            return; // В этом узле нет виджета-кнопки
        }

        // 3. Логика для SINGLE (Radio) режима
        if (type != ButtonGroupType::MULTI && state == true)
        {
            for (auto& child : children)
            {
                if (auto* btn = getButtonFromNode(child.get()))
                {
                    if (btn != targetButton && btn->getIsChecked())
                    {
                        btn->setIsChecked(false);
                    }
                }
            }
        }

        // 4. Устанавливаем состояние целевой кнопки
        targetButton->setIsChecked(state);
    }


    void ButtonGroup::setType(ButtonGroupType type) noexcept
    {
        this->type = type;
    }
}