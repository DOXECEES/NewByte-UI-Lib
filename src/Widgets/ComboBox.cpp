#include "ComboBox.hpp"

#include "Indentations.hpp"

namespace Widgets
{
	ComboBox::ComboBox() noexcept
		: IWidget({})
		, dropdownList(std::make_unique<DropdownList>())
	{
		this->addChildrenWidget(dropdownList.get());
		subscribe(static_cast<IWidget*>(this), &IWidget::onSizeChangedSignal, [this](const NbRect<int>& rc)
		{
			Padding p;

			selectedItemRect = {
				rc.x,
				rc.y,
				rc.width - 20,
				20,
			};

			buttonRect = {
				rc.x + selectedItemRect.width,
				rc.y,
				20,
				20
			};

			dropdownList->setRect({
				rc.x,
				rc.y + 20,
				rc.width,
				20 * static_cast<int>(dropdownList->size())
			});
			//selectedItemRect = applyLeftTopPaddingToRect(selectedItemRect, p);
			//buttonRect = applyLeftTopPaddingToRect(buttonRect, p);

		});
	}

	bool ComboBox::hitTest(const NbPoint<int>& pos)
	{
		return pos.x >= rect.x && pos.x < rect.x + rect.width 
			&& pos.y >= rect.y && pos.y < rect.y + rect.height;
	}

	bool ComboBox::hitTestClick(const NbPoint<int>& pos) noexcept
	{
		bool flag = buttonRect.isInside(pos);
		if (flag)
		{
			toggleComboState();
		}

		return flag;
	}

	NB_NODISCARD const NbRect<int>& ComboBox::getButtonRect() const noexcept
	{
		return buttonRect;
	}

	NB_NODISCARD const NbRect<int>& ComboBox::getSelectedItemRect() const noexcept
	{
		return selectedItemRect;
	}

	const NbRect<int>& ComboBox::getDropdownRect() const noexcept
	{
		return dropdownList->getRect();
	}

	void ComboBox::addItem(const ListItem& item) noexcept
	{
		dropdownList->add(item);
	}

	NB_NODISCARD ComboBox::ComboState ComboBox::getComboState() const noexcept
	{
		return comboBoxState;
	}

	NB_NODISCARD size_t ComboBox::getSize() const noexcept
	{
		return dropdownList->size();
	}

	const std::vector<ListItem>& ComboBox::getAllItems() const noexcept
	{
		return dropdownList->getAllItems();
	}

	NbRect<int> ComboBox::getRequestedSize() const noexcept
	{
		NbRect<int> requestedRect;

		requestedRect = {
			0,
			0,
			200,
			std::max(selectedItemRect.height, buttonRect.height)
		};

		return requestedRect;
		//return applyScaleOnlyPadding(requestedRect, Padding());
	}

	void ComboBox::closeAllDropDowns() noexcept
	{
		if (openedComboBox)
		{
			openedComboBox->comboBoxState = ComboState::COLLAPSED;
			openedComboBox = nullptr;
		}
	}

	void ComboBox::toggleComboState() noexcept
	{
		if (openedComboBox != this)
		{
			closeAllDropDowns();
			openedComboBox = this;
		}

		switch (comboBoxState)
		{
			case ComboState::EXPANDED:
			{
				comboBoxState = ComboState::COLLAPSED;
				break;
			}
			case ComboState::COLLAPSED:
			{
				comboBoxState = ComboState::EXPANDED;
				break;
			}
			default:
				break;
		}

	}

	DropdownList::DropdownList() noexcept
		: IWidget({}, 1)
	{
	
	}

	void DropdownList::add(ListItem item) noexcept
	{
		itemList.push_back(item);
		rect = {
			rect.x,
			rect.y,
			rect.width,
			20 * static_cast<int>(itemList.size())
		};
	}

	NB_NODISCARD size_t DropdownList::size() const noexcept
	{
		return itemList.size();
	}

	NB_NODISCARD const std::vector<ListItem>& DropdownList::getAllItems() const noexcept
	{
		return itemList;
	}

	bool DropdownList::hitTest(const NbPoint<int>& pos)
	{
		return rect.isInside(pos);
	}

	const char* DropdownList::getClassName() const
	{
		return CLASS_NAME;
	}
	

	ListItem::ListItem(std::wstring_view str) noexcept
		: text(str)
	{}

	NB_NODISCARD const std::wstring& ListItem::getText() const noexcept
	{
		return text;
	}

};
