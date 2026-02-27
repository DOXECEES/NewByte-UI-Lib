// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
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
            hDefer = DeferWindowPos(hDefer, handle.as<HWND>(), nullptr, rect.x + 5, rect.y + 35, rect.width, rect.height, flags);
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

