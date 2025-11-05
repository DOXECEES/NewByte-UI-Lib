#include "WindowPositionQueue.hpp"

namespace Utils
{
	namespace Windows
	{
		WindowPosQueue::~WindowPosQueue() noexcept
        {
            if (hDefer)
            {
                EndDeferWindowPos(hDefer);
                hDefer = nullptr;
            }
        }

        void WindowPosQueue::begin(size_t maxCount) noexcept
        {
            if (hDefer)
            {
                EndDeferWindowPos(hDefer);
            }
            hDefer = BeginDeferWindowPos(static_cast<int>(maxCount));
        }

        void WindowPosQueue::push(const NbWindowHandle& handle, const NbRect<int>& rect, UINT flags) noexcept
        {
            if (!hDefer)
            {
                return;
            }
            hDefer = DeferWindowPos(hDefer, handle.as<HWND>(), nullptr, rect.x, rect.y, rect.width, rect.height, flags);
        }

        void WindowPosQueue::apply() noexcept
        {
            if (!hDefer)
            {
                return;
            }
            EndDeferWindowPos(hDefer);
            hDefer = nullptr;
        }




	};
};

