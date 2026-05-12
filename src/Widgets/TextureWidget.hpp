#ifndef NBUI_SRC_WIDGETS_TEXTURE_WIDGET_HPP
#define NBUI_SRC_WIDGETS_TEXTURE_WIDGET_HPP

#include "Widgets/IWidget.hpp"
#include "Widgets/Label.hpp"
#include <string>

#include <Error/ErrorManager.hpp>

namespace Widgets
{
    class TextureWidget : public IWidget
    {
    public:
        TextureWidget() : IWidget({})
        {
            nameLabel->setText(L"None (Texture)");
            nameLabel->getStyle().font.setSize(9);
            nameLabel->getStyle().alignment = {TextAlignment::LEFT, ParagraphAlignment::CENTER};

            resolutionLabel->setText(L"Texture Asset");
            resolutionLabel->getStyle().font.setSize(7);
            resolutionLabel->getStyle().baseTextColor = {150, 150, 150}; 
            resolutionLabel->getStyle().alignment     = {TextAlignment::LEFT, ParagraphAlignment::CENTER};
        }

        DECLARE_WIDGET_CLASS_NAME(TextureWidget);

        void setTexture(
            const std::wstring& name,
            const std::wstring& resolution = L"Unknown size",
            bool                isAssigned = true
        )
        {
            this->isAssigned = isAssigned;
            if (isAssigned)
            {
                nameLabel->setText(name);
                resolutionLabel->setText(resolution);
            }
            else
            {
                nameLabel->setText(L"None (Texture)");
                resolutionLabel->setText(L"Texture Asset");
            }
        }

        bool hitTest(const NbPoint<int>& pos) override
        {
            return rect.isInside(pos);
        }

        void onClick() override
        {
            onClickSignal.emit();
        }

        const char* getClassName() const override
        {
            return CLASS_NAME;
        }

        const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
        {
            measuredSize = {maxSize.width, 50};
            return measuredSize;
        }

        void layout(const NbRect<int>& rect) noexcept override
        {
            setRect(rect);

            const int padding = 5;
            int iconSize = rect.height - (padding * 2);
            previewRect  = {rect.x + padding, rect.y + padding, iconSize, iconSize};

            int textX       = previewRect.x + previewRect.width + padding;
            int textWidth   = (std::max)(0, rect.width - previewRect.width - (padding * 3));
            int labelHeight = iconSize / 2;

            NbRect<int> nameRect = {textX, rect.y + padding, textWidth, labelHeight};
            nameLabel->layout(nameRect);

            NbRect<int> typeRect = {textX, rect.y + padding + labelHeight, textWidth, labelHeight};
            resolutionLabel->layout(typeRect);
        }

        NbRect<int> getPreviewRect() const noexcept
        {
            return previewRect;
        }
        bool isTextureAssigned() const noexcept
        {
            return isAssigned;
        }

        const std::shared_ptr<Label>& getNameLabel() const noexcept
        {
            return nameLabel;
        }
        const std::shared_ptr<Label>& getResolutionLabel() const noexcept
        {
            return resolutionLabel;
        }

        Signal<void()> onClickSignal;

    private:
        std::shared_ptr<Label> nameLabel   = std::make_shared<Label>(L"");
        std::shared_ptr<Label> resolutionLabel   = std::make_shared<Label>(L"");
        NbRect<int>            previewRect = {0, 0, 0, 0};
        bool                   isAssigned  = false;
    };
} // namespace Widgets

#endif