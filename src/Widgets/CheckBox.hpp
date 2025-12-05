#ifndef NBUI_SRC_WIDGETS_CHECKBOX_HPP
#define NBUI_SRC_WIDGETS_CHECKBOX_HPP

#include <Alghorithm.hpp>
#include "IWidget.hpp"
#include "Widgets/Label.hpp"

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
	
        virtual const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
        {
            constexpr int checkBoxSize = 16;   // размер квадратика
            constexpr int charWidth = 50;      // ширина одного символа
            constexpr int lineHeight = 20;     // высота строки
            constexpr int paddingLeft = 4;
            constexpr int paddingRight = 4;
            constexpr int paddingTop = 2;
            constexpr int paddingBottom = 2;
            constexpr int spacing = 4;         // расстояние между квадратиком и текстом

            int textWidth = static_cast<int>(label->getText().size()) * charWidth;
            int width = checkBoxSize + spacing + textWidth + paddingLeft + paddingRight;
            int height = (nbstl::max)(lineHeight, checkBoxSize) + paddingTop + paddingBottom;

            width = (nbstl::min)(width, maxSize.width);
            height = (nbstl::min)(height, maxSize.height);

            size.width = width;
            size.height = height;

            return size;
        }

        virtual void layout(const NbRect<int>& rect) noexcept override
        {
            this->rect = rect;

            // Позиция квадратика внутри rect
            boxRect.x = rect.x + 4; // paddingLeft
            boxRect.y = rect.y + (rect.height - 16) / 2;
            boxRect.width = 16;
            boxRect.height = 16;

            // Позиция текста
            NbRect<int> textRect;

            textRect.x = boxRect.x + boxRect.width + 4; // spacing
            textRect.y = rect.y + 2; // paddingTop
            textRect.width = rect.width - textRect.x + rect.x - 4; // остаток ширины
            textRect.height = rect.height - 4; // paddingTop + paddingBottom
            label->setRect(textRect);
        }
	


	public:
		Signal<void(bool)> onCheckStateChanged;

	private:

		bool					isChecked			= false;
		bool					isIndeterminate		= true;

		NbRect<int>				boxRect;
		std::unique_ptr<Label>	label				= nullptr;	

        NbSize<int>     size;


	};
};

#endif 
