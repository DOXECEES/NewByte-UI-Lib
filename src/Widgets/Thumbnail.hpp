#ifndef NBUI_SRC_WIDGETS_THUMBNAIL_HPP
#define NBUI_SRC_WIDGETS_THUMBNAIL_HPP

#include "Core.hpp"
#include "IWidget.hpp"
#include "Widgets/IWidget.hpp"

#include "Widgets/Label.hpp"
#include <oaidl.h>
#include <string>

namespace Widgets
{
	class Thumbnail : public IWidget
	{

    public:
		Thumbnail(const std::wstring& name, const std::wstring& type)
			:IWidget({})
        {
            typeLabel->setText(type);
            nameLabel->setText(name);
            nameLabel->setEllipsis(true);
            
			typeLabel->getStyle().alignment = {TextAlignment::LEFT, ParagraphAlignment::CENTER};
			typeLabel->getStyle().font.setSize(8);
		}

		DECLARE_WIDGET_CLASS_NAME(Thumbnail);

    	bool hitTest(const NbPoint<int>& pos) override
    	{
			return rect.isInside(pos);
		}
    	const char* getClassName() const override
    	{
    	    return CLASS_NAME;
    	}

		const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
        {
            measuredSize = {400, 400};

            return measuredSize;
        }

        void layout(const NbRect<int>& rect) noexcept override
        {
            setRect(rect);

            //NbRect<int> nameLabelRect = {
            //    rect.x,
            //    rect.y + rect.height - 45,
            //    rect.width,
            //    30
            //};

            //nameLabel->setRect(nameLabelRect);

            //NbRect<int> typeLabelRect = {
            //    rect.x,
            //    rect.y + rect.height - 15,
            //    rect.width,
            //    15
            //};

            //typeLabel->setRect(typeLabelRect);
        }

        std::shared_ptr<Label> getNameLabel() const noexcept
        {
            return nameLabel;
        }

        std::shared_ptr<Label> getTypeLabel() const noexcept
        {
            return typeLabel;
        }

		NbRect<int> getDrawRect() const noexcept
		{
			NbRect<int> rc = {
                rect.x,
                rect.y,
                rect.width,
				rect.height
            };

			return rc;
		}

        const std::wstring& getName() const noexcept
        {
            return nameLabel->getText();
        }

        const std::wstring& getType() const noexcept
        {
            return typeLabel->getText();
        }

    private:
        std::shared_ptr<Label> nameLabel = std::make_shared<Label>(L"Some name");
        std::shared_ptr<Label> typeLabel = std::make_shared<Label>(L"Some type");

	};
};

#endif