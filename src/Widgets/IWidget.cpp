#include "IWidget.hpp"

namespace Widgets
{
	void IWidget::hide() noexcept
	{
		isHide_ = true;
	}

	void IWidget::show() noexcept
	{
		isHide_ = false;
	}

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

	bool IWidget::isHide() const noexcept
	{
		return isHide_;
	}

	NbRect<int> IWidget::getRequestedSize() const noexcept
	{
		return {};
	}

	void IWidget::addChildrenWidget(IWidget* widget) noexcept
	{
		childrens.push_back(widget);
	}

	const std::vector<IWidget*>& IWidget::getChildrens() const noexcept
	{
		return childrens;
	}

	const Core::ZIndex& IWidget::getZIndex() const noexcept
	{
		return zIndex;
	}

};