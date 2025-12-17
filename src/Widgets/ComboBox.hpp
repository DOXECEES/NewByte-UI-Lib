#ifndef NBUI_SRC_WIDGETS_COMBOBOX_HPP
#define NBUI_SRC_WIDGETS_COMBOBOX_HPP

#include "NbCore.hpp"

#include "IWidget.hpp"

#include <vector>

namespace Widgets
{
	class ListItem
	{
	public:
		ListItem(std::wstring_view str) noexcept;
		
		NB_NODISCARD const std::wstring& getText() const noexcept;
	private:
		std::wstring	text;
	};

	class DropdownList : public IWidget
	{
		
	public:

		static constexpr const char* CLASS_NAME					= "ComboBoxDropDownList";
		static constexpr int		 SIZE_OF_ELEMENT_IN_PIXEL	= 20;

		DropdownList() noexcept;
		~DropdownList() noexcept = default;

		DropdownList(const DropdownList&) = delete;
		DropdownList& operator=(const DropdownList&) = delete;

		DropdownList(DropdownList&&) noexcept = default;
		DropdownList& operator=(DropdownList&&) noexcept = default;


		void add(ListItem item) noexcept;
		NB_NODISCARD size_t size() const noexcept;
		NB_NODISCARD const std::vector<ListItem>& getAllItems() const noexcept;

		bool hitTest(const NbPoint<int>& pos) override;
		bool hitTestClick(const NbPoint<int>& pos) noexcept override;
		NB_NODISCARD size_t hitTestElement(const NbPoint<int>& pos) const noexcept;
		const char* getClassName() const override;

		void setHoverForElement(const size_t hoverIndex) noexcept;
		NB_NODISCARD size_t getHoverElementIndex() const noexcept;
		NB_NODISCARD NbRect<int> getHoverElementRect() const noexcept;

		virtual const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
		{
			return {0, 0};
		}

		virtual void layout(const NbRect<int>& rect) noexcept override
		{
			this->rect = {
				this->rect.x,
				this->rect.y,
				this->rect.width,
				SIZE_OF_ELEMENT_IN_PIXEL * static_cast<int>(itemList.size())
			};
		}

	private:
		std::vector<ListItem>	itemList = {
			{L"Hello"},
			{L"HI" },
			{L"Whatsup"},
			{L"Sup"}
		};

		size_t					hoverElement = 0;
	};

	class ComboBox : public IWidget
	{
	public:

		enum class ComboState
		{
			EXPANDED,
			COLLAPSED,
		};

		static constexpr const char*	CLASS_NAME									= "ComboBox";
		static constexpr int			HEIGHT_OF_SELECTED_ITEM_ELEMENT_IN_PIXEL	= 20;
		static constexpr int			WIDTH_OF_BUTTON_ELEMENT_IN_PIXEL			= 20;
		static constexpr int			HEIGHT_OF_BUTTON_ELEMENT_IN_PIXEL			= 20;

		ComboBox() noexcept;
		~ComboBox() noexcept = default;

		bool hitTest(const NbPoint<int>& pos) override;
		bool hitTestClick(const NbPoint<int>& pos) noexcept override;

		const char* getClassName() const override { return CLASS_NAME; }

		NB_NODISCARD const NbRect<int>& getButtonRect() const noexcept;
		NB_NODISCARD const NbRect<int>& getSelectedItemRect() const noexcept;
		NB_NODISCARD const NbRect<int>& getDropdownRect() const noexcept;

		void addItem(const ListItem& item) noexcept;

		NB_NODISCARD ComboState getComboState() const noexcept;
		NB_NODISCARD size_t getSize() const noexcept;
		NB_NODISCARD const std::vector<ListItem>& getAllItems() const noexcept;

		NbRect<int> getRequestedSize() const noexcept override;

		void closeAllDropDowns() noexcept;

		virtual const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
		{
			constexpr int charWidth = 50;
			constexpr int lineHeight = 20;
			constexpr int paddingLeft = 4;
			constexpr int paddingRight = 4;
			constexpr int paddingTop = 2;
			constexpr int paddingBottom = 2;

			//int width = static_cast<int>(text.size()) * charWidth + paddingLeft + paddingRight;
			//int height = lineHeight + paddingTop + paddingBottom;
			//
			//width = (nbstl::min)(width, maxSize.width);
			//height = (nbstl::min)(height, maxSize.height);

			size.width = 100;
			size.height = 30;

			return size;
		}

		void layout(const NbRect<int>& rect) noexcept override
		{
			this->rect = rect;

			constexpr int buttonWidth = 20; // WIDTH_OF_BUTTON_ELEMENT_IN_PIXEL = 20

			// 1. Кнопка со стрелкой (справа)
			buttonRect = NbRect<int>{
				rect.x + rect.width - buttonWidth, // справа
				rect.y,
				buttonWidth,
				rect.height
			};

			selectedItemRect = NbRect<int>{
				rect.x,
				rect.y,
				rect.width - buttonWidth, 
				rect.height
			};

			if (comboBoxState == ComboState::EXPANDED && dropdownList)
			{
				updateDropdownPosition();
			}
		}

		void updateDropdownPosition()
		{
			NbRect<int> newRc = {
				rect.x,
				rect.y + rect.height,
				rect.width,
				0
			};
			dropdownList->setRect(newRc);
		}

	private:

		void toggleComboState() noexcept;

	private:
		NbRect<int>						buttonRect;
		NbRect<int>						selectedItemRect;

		std::unique_ptr<DropdownList>	dropdownList;
		ComboState						comboBoxState = ComboState::COLLAPSED;
	
		inline static ComboBox*			openedComboBox = nullptr;

		NbSize<int> size;
	
	};

};

#endif