#include "IWindow.hpp"

namespace WindowInterface
{
	void IWindow::notifyAllListeners() noexcept
	{
		for (const auto& listener : stateChangedListeners)
		{
			listener->onSizeChanged(state.clientSize);
		}
	}

};