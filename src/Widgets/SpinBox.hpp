#ifndef NBUI_SRC_WIDGETS_SPINBOX_HPP
#define NBUI_SRC_WIDGETS_SPINBOX_HPP

#include "IWidget.hpp"

namespace Widgets
{
	class SpinBox: public IWidget
	{
	public:
		
		void setStep(const int step) noexcept;
		void setMinValue(const int minValue) noexcept;
		void setMaxValue(const int maxValue) noexcept;
		void setValue(const int value) noexcept;

		NB_NODISCARD int getStep() const noexcept;
		NB_NODISCARD int getMinValue() const noexcept;
		NB_NODISCARD int getMaxValue() const noexcept;
		NB_NODISCARD int getValue() const noexcept;
	
	public:

		Signal<void(int)> onValueChanged;
	
	private:

		bool hitTest(const NbPoint<int>& pos) override;
		const char* getClassName() const override;

		int step;
		int minValue;
		int maxValue;
		int value;

	};
};


#endif