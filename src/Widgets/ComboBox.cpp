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
				rc.width - WIDTH_OF_BUTTON_ELEMENT_IN_PIXEL,
				HEIGHT_OF_SELECTED_ITEM_ELEMENT_IN_PIXEL,
			};

			buttonRect = {
				rc.x + selectedItemRect.width,
				rc.y,
				WIDTH_OF_BUTTON_ELEMENT_IN_PIXEL,
				HEIGHT_OF_BUTTON_ELEMENT_IN_PIXEL
			};

			dropdownList->setRect({
				rc.x,
				rc.y + HEIGHT_OF_BUTTON_ELEMENT_IN_PIXEL,
				rc.width,
				DropdownList::SIZE_OF_ELEMENT_IN_PIXEL * static_cast<int>(dropdownList->size())
			});
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
			rect.width,
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
			SIZE_OF_ELEMENT_IN_PIXEL * static_cast<int>(itemList.size())
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
		bool flag = rect.isInside(pos);
		if (flag)
		{
			size_t elementIndex = hitTestElement(pos);
			setHoverForElement(elementIndex);
		}
		return flag;
	}

	size_t DropdownList::hitTestElement(const NbPoint<int>& pos) const noexcept
	{
		return (pos.y - rect.y) / SIZE_OF_ELEMENT_IN_PIXEL;
	}

	const char* DropdownList::getClassName() const
	{
		return CLASS_NAME;
	}

	size_t DropdownList::getHoverElementIndex() const noexcept
	{
		return hoverElement;
	}

	NbRect<int> DropdownList::getHoverElementRect() const noexcept
	{
		return {
			rect.x,
			rect.y + SIZE_OF_ELEMENT_IN_PIXEL * static_cast<int>(hoverElement),
			rect.width,
			SIZE_OF_ELEMENT_IN_PIXEL
		};
	}


	void DropdownList::setHoverForElement(const size_t hoverIndex) noexcept
	{
		hoverElement = hoverIndex;
	}
	

	ListItem::ListItem(std::wstring_view str) noexcept
		: text(str)
	{}


	NB_NODISCARD const std::wstring& ListItem::getText() const noexcept
	{
		return text;
	}

};
