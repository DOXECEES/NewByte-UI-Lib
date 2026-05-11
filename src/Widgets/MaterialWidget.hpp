#ifndef NBUI_SRC_WIDGETS_MATERIAL_WIDGET_HPP
#define NBUI_SRC_WIDGETS_MATERIAL_WIDGET_HPP

#include "Widgets/IWidget.hpp"
#include "Widgets/Label.hpp"
#include <string>

#include <Error/ErrorManager.hpp>

namespace Widgets
{
    class MaterialWidget : public IWidget
    {
    public:
        MaterialWidget() : IWidget({})
        {
            nameLabel->setText(L"None (Material)");
            nameLabel->getStyle().font.setSize(9);
            nameLabel->getStyle().activeColor = {100, 100, 100};
            nameLabel->getStyle().alignment = {TextAlignment::LEFT, ParagraphAlignment::CENTER};

            typeLabel->setText(L"Material Asset");
            typeLabel->getStyle().font.setSize(7);
            typeLabel->getStyle().activeColor     = {100, 100, 100};
            typeLabel->getStyle().alignment = {TextAlignment::LEFT, ParagraphAlignment::CENTER};
        }

        DECLARE_WIDGET_CLASS_NAME(MaterialWidget);

        void setMaterial(
            const std::wstring& name,
            bool                isAssigned = true
        )
        {
            nameLabel->setText(isAssigned ? name : L"None (Material)");
            this->isAssigned = isAssigned;
        }

        bool hitTest(const NbPoint<int>& pos) override
        {
            return rect.isInside(pos);
        }

        void onClick() override
        {
            nb::Error::ErrorManager::instance().report(nb::Error::Type::INFO, "Click");
            onClickSignal.emit();
        }

        const char* getClassName() const override
        {
            return CLASS_NAME;
        }

        const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
        {
            measuredSize = {maxSize.width, 45};
            return measuredSize;
        }

        void layout(const NbRect<int>& rect) noexcept override
        {
            setRect(rect);

            int iconSize = rect.height - 10;
            previewRect  = {rect.x + 5, rect.y + 5, iconSize, iconSize};

            int textX     = previewRect.x + previewRect.width + 5;
            int textWidth = rect.width - previewRect.width - 10 - 5;

            nameLabel->setRect({textX, rect.y + 5, textWidth, iconSize / 2});
            typeLabel->setRect({textX, rect.y + (iconSize / 2) + 5, textWidth, iconSize / 2});
        }

        NbRect<int> getPreviewRect() const noexcept
        {
            return previewRect;
        }

        bool isMaterialAssigned() const noexcept
        {
            return isAssigned;
        }

        const std::shared_ptr<Label>& getNameLabel() const noexcept
        {
            return nameLabel;
        }
                
        const std::shared_ptr<Label>& getTypeLabel() const noexcept
        {
            return typeLabel;
        }

        Signal<void()> onClickSignal;

    private:
        std::shared_ptr<Label> nameLabel   = std::make_shared<Label>(L"");
        std::shared_ptr<Label> typeLabel   = std::make_shared<Label>(L"");
        NbRect<int>            previewRect = {0, 0, 0, 0};
        bool                   isAssigned  = false;
    };
} // namespace Widgets

#endif