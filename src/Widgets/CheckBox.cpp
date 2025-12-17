#include "CheckBox.hpp"

#include "Label.hpp"
#include "Indentations.hpp"

namespace Widgets
{
	CheckBox::CheckBox() noexcept
		: IWidget({})
		, label(std::make_unique<Label>())
	{
		sizePolicy.horizontal = SizePolicy::EXPANDING;
		sizePolicy.vertical = SizePolicy::FIXED;
		label->setVTextAlign(Label::VTextAlign::LEFT);
		label->setHTextAlign(Label::HTextAlign::CENTER);

	}

	CheckBox::CheckBox(const NbRect<int>& rect) noexcept
		: IWidget({})
		, label(std::make_unique<Label>())
	{
		sizePolicy.horizontal = SizePolicy::EXPANDING;
		sizePolicy.vertical = SizePolicy::FIXED;
		label->setVTextAlign(Label::VTextAlign::LEFT);
		label->setHTextAlign(Label::HTextAlign::CENTER);

		subscribe(static_cast<IWidget*>(this), &IWidget::onSizeChangedSignal, [this](const NbRect<int>& rc)
		{
			OldPadding p;

			boxRect = {
				rc.x,
				rc.y,
				20,
				20,
			};

			label->setRect(applyLeftTopPaddingToRect({
				boxRect.x + boxRect.width + p.right,
				boxRect.y,
				this->rect.width,
				boxRect.height
			}, p));

			boxRect = applyLeftTopPaddingToRect(boxRect, p);

		});
	}


	bool CheckBox::hitTest(const NbPoint<int>& pos)
	{
		return pos.x >= boxRect.x && pos.x < boxRect.x + boxRect.width 
			&& pos.y >= boxRect.y && pos.y < boxRect.y + boxRect.height;
	}

	bool CheckBox::hitTestClick(const NbPoint<int>& pos) noexcept
	{
		bool flag = hitTest(pos);
		if (flag) toogleIsChecked();
		return flag;
	}

	bool CheckBox::getIsChecked() const noexcept
	{
		return isChecked;
	}

	void CheckBox::toogleIsChecked() noexcept
	{
		isChecked = !isChecked;
		onCheckStateChanged.emit(isChecked);
	}

	const NbRect<int>& CheckBox::getBoxRect() const noexcept
	{
		return boxRect;
	}

	Widgets::Label* CheckBox::getLabel() const noexcept
	{
		return label.get();
	}

	void CheckBox::setText(const std::wstring& text) const noexcept
	{
		label->setText(text);
	}

	NbRect<int> CheckBox::getRequestedSize() const noexcept
	{
		NbRect<int> requestedRect;
		const NbRect<int>& labelRect = label->getRect();
		
		requestedRect = {
			0,
			0,
			200,
			std::max(boxRect.height, labelRect.height)
		};


		return applyScaleOnlyPadding(requestedRect, OldPadding());
	}

};



