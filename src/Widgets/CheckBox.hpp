#ifndef NBUI_SRC_WIDGETS_CHECKBOX_HPP
#define NBUI_SRC_WIDGETS_CHECKBOX_HPP

#include "IWidget.hpp"

#include <memory>

namespace Widgets
{
	class Label;

	class CheckBox : public IWidget
	{
	public:

		static constexpr const char* CLASS_NAME = "CheckBox";


		CheckBox(const NbRect<int>& rect) noexcept;
		virtual ~CheckBox() noexcept = default;

		bool hitTest(const NbPoint<int>& pos) override;
		bool hitTestClick(const NbPoint<int>& pos) noexcept override; 

		const char* getClassName() const noexcept override { return CLASS_NAME; }

		bool getIsChecked() const noexcept;
		void toogleIsChecked() noexcept;
		
		const NbRect<int>& getBoxRect() const noexcept;

		Label* getLabel() const noexcept;

		void setText(const std::wstring& text) const noexcept;

		NbRect<int> getRequestedSize() const noexcept override;
	
		
	


	public:
		Signal<void(bool)> onCheckStateChanged;

	private:

		bool					isChecked			= false;
		bool					isIndeterminate		= true;

		NbRect<int>				boxRect;
		std::unique_ptr<Label>	label				= nullptr;	



	};
};

#endif 
