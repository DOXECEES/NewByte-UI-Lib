#ifndef NBUI_SRC_WIDGETS_COMBOBOX_HPP
#define NBUI_SRC_WIDGETS_COMBOBOX_HPP

#include "NbCore.hpp"

#include "IWidget.hpp"

#include <vector>
#include <Vector.hpp>
#include <any>

namespace Widgets
{
	class ListItem
	{
	public:
		template<typename T>
		ListItem(std::wstring_view label, T&& val) noexcept
			: text(label), value(std::forward<T>(val))
		{}

		NB_NODISCARD const std::wstring& getText() const noexcept { return text; }
		
		template<typename T>
		NB_NODISCARD T getValue() const { return std::any_cast<T>(value); }

	private:
		std::wstring	text;
		std::any		value;
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

		const ListItem& getListItem() const noexcept;

		virtual const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
		{
			return {0, 0};
		}

		virtual void layout(const NbRect<int>& rect) noexcept override
		{
			this->rect = {
				rect.x,
				rect.y,
				rect.width,
				SIZE_OF_ELEMENT_IN_PIXEL * static_cast<int>(itemList.size())
			};
		}

		Signal<void(const ListItem&)> onItemChecked;

	private:
		/*std::vector<ListItem>	itemList = {
			{L"Hello"},
			{L"HI" },
			{L"Whatsup"},
			{L"Sup"}
		};*/
		std::vector<ListItem> itemList;

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

		const ListItem& getSelectedItem() const noexcept;

		NbRect<int> getRequestedSize() const noexcept override;

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

		// В ComboBox::layout
		void layout(const NbRect<int>& rect) noexcept override {
			this->rect = rect;

			buttonRect = NbRect<int>{
				rect.x + rect.width - WIDTH_OF_BUTTON_ELEMENT_IN_PIXEL,
				rect.y,
				WIDTH_OF_BUTTON_ELEMENT_IN_PIXEL,
				rect.height
			};

			selectedItemRect = NbRect<int>{
				rect.x,
				rect.y,
				rect.width - WIDTH_OF_BUTTON_ELEMENT_IN_PIXEL,
				rect.height
			};

			if (dropdownList) {
				dropdownList->setRect({
					rect.x,
					rect.y + rect.height, // Сразу под комбобоксом
					rect.width,
					DropdownList::SIZE_OF_ELEMENT_IN_PIXEL * static_cast<int>(dropdownList->size())
					});
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

		static void registerDropdown(DropdownList* dropdown) noexcept;
		static void closeAllDropDowns() noexcept;

	public:

		Signal<void(const ListItem&)> onItemChecked;

	private:

		void toggleComboState() noexcept;
		


	private:
		NbRect<int>						buttonRect;
		NbRect<int>						selectedItemRect;

		std::unique_ptr<DropdownList>	dropdownList;
		ComboState						comboBoxState = ComboState::COLLAPSED;
	
		inline static ComboBox*			openedComboBox = nullptr;

		NbSize<int> size;
	
		inline static std::unordered_map<int, DropdownList*> dropdowns;
	};

};

#endif