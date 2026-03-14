#ifndef NBUI_SRC_WIDGETS_THUMBNAIL_HPP
#define NBUI_SRC_WIDGETS_THUMBNAIL_HPP

#include "Core.hpp"
#include "IWidget.hpp"
#include "Widgets/IWidget.hpp"

#include "Widgets/Label.hpp"
#include <oaidl.h>

namespace Widgets
{
	class Thumbnail : public IWidget
	{

    public:
		Thumbnail()
			:IWidget({})
        {
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

            NbRect<int> nameLabelRect = {
                rect.x,
                rect.y + rect.height - 45,
                rect.width,
                30
            };

            nameLabel->setRect(nameLabelRect);

            NbRect<int> typeLabelRect = {
                rect.x,
                rect.y + rect.height - 15,
                rect.width,
                15
            };

            typeLabel->setRect(typeLabelRect);
        }

        std::shared_ptr<Label> getNameLabel() const noexcept
        {
            return nameLabel;
        }

        std::shared_ptr<Label> getTypeLabel() const noexcept
        {
            return typeLabel;
        }

		const NbRect<int>& getDrawRect() const noexcept
		{
			NbRect<int> rc = {
                rect.x,
                rect.y,
                rect.width,
				rect.height - 45
            };

			return rc;
		}


    private:
        std::shared_ptr<Label> nameLabel = std::make_shared<Label>(L"Some name");
        std::shared_ptr<Label> typeLabel = std::make_shared<Label>(L"Some type");

	};
};

#endif