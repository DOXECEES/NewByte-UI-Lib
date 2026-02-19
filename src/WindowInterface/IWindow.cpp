// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "IWindow.hpp"

namespace WindowInterface
{
	IWindow::IWindow() noexcept
	{
		rootLayout = std::make_unique<NNsLayout::LayoutWindow>(this);
	}

	void IWindow::notifyAllListeners() noexcept
	{
		for (const auto& listener : stateChangedListeners)
		{
			listener->onSizeChanged(state.clientSize);
		}
	}

	void IWindow::recalculateLayout() noexcept
	{
		if (!rootLayout)
		{
			return;
		}

		if (rootLayout->isDirty()) 
		{
			NbSize<int> available = getClientSize();
			rootLayout->measure(available);
			rootLayout->layout({ 0, 0, available.width, available.height });
		}

	}

};