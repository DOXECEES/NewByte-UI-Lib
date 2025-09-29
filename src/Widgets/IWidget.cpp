#include "IWidget.hpp"

namespace Widgets
{
	bool IWidget::isHover() const noexcept
	{
		return state == WidgetState::HOVER;
	}


	bool IWidget::isActive() const noexcept
	{
		return state == WidgetState::ACTIVE;
	}


	bool IWidget::isDisable() const noexcept
	{
		return state == WidgetState::DISABLE;
	}


	bool IWidget::isDefault() const noexcept
	{
		return state == WidgetState::DEFAULT;
	}

	NbRect<int> IWidget::getRequestedSize() const noexcept
	{
		return {};
	}

};