// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "IWidget.hpp"
#include "Menu.hpp"
#include "PopUp.hpp"

namespace Widgets
{
	void IWidget::hide() noexcept
	{
		isHide_ = true;
	}

	void IWidget::onClick()
    {
        if (state == WidgetState::DISABLE)
        {
            return;
        }
        if (onClickCallback)
        {
            onClickCallback();
        }
        if (contextMenu)
        {
            dynamic_cast<Menu*>(contextMenu)->hideMenu();
        }
        onPressedSignal.emit();
    };

    void IWidget::show() noexcept
	{
		isHide_ = false;
	}

	void IWidget::hide(bool flag) noexcept
    {
        isHide_ = flag;
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

    void IWidget::disableHoverState(bool flag) noexcept
    {
        isDisableHoverState = flag;
    }

    bool IWidget::isHoverStateDisable() const noexcept
    {
        return isDisableHoverState;
    }

	void IWidget::setFocused() noexcept
	{
		isFocused = true;
		onFocusSignal.emit();
	}

	void IWidget::setUnfocused() noexcept
	{
		isFocused = false;
		//onUnfocusedSignal.emit();
	}

	NbRect<int> IWidget::getRequestedSize() const noexcept
	{
		return {};
	}

	void IWidget::addChildrenWidget(std::shared_ptr<IWidget> widget) noexcept
	{
		childrens.push_back(std::move(widget));
	}

	const std::vector<std::shared_ptr<IWidget>>& IWidget::getChildrens() const noexcept
	{
		return childrens;
	}

	const Core::ZIndex& IWidget::getZIndex() const noexcept
	{
		return zIndex;
    }

    void IWidget::onRightClick(const NbPoint<int>& point) noexcept
    {
        if (contextMenu)
        {
            //dynamic_cast<Menu*>(contextMenu)->showMenu(point.x, point.y);
            //PopupMenuManager manager;
			//manager
		}
    }

};