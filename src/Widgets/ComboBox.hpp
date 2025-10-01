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
		NB_NODISCARD size_t hitTestElement(const NbPoint<int>& pos) const noexcept;
		const char* getClassName() const override;

		void setHoverForElement(const size_t hoverIndex) noexcept;
		NB_NODISCARD size_t getHoverElementIndex() const noexcept;
		NB_NODISCARD NbRect<int> getHoverElementRect() const noexcept;

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

	private:

		void toggleComboState() noexcept;

	private:
		NbRect<int>						buttonRect;
		NbRect<int>						selectedItemRect;

		std::unique_ptr<DropdownList>	dropdownList;
		ComboState						comboBoxState = ComboState::COLLAPSED;
	
		inline static ComboBox*			openedComboBox = nullptr;
	
	};

};

#endif