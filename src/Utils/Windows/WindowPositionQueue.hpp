#ifndef NBUI_SRC_UTILS_WINDOWS_WINDOWPOSITIONQUEUE_HPP
#define NBUI_SRC_UTILS_WINDOWS_WINDOWPOSITIONQUEUE_HPP

#include <Windows.h>

#include "Core.hpp"
#include "WindowInterface/WindowCore.hpp"

namespace Utils
{
    namespace Windows
    {
        class WindowPosQueue
        {
        public:
            WindowPosQueue() noexcept = default;
            ~WindowPosQueue() noexcept;

            WindowPosQueue(const WindowPosQueue&) noexcept = delete;
            WindowPosQueue& operator=(const WindowPosQueue&) noexcept = delete;

            WindowPosQueue(WindowPosQueue&&) noexcept = delete;
            WindowPosQueue& operator=(WindowPosQueue&&) noexcept = delete;

            void begin(size_t maxCount) noexcept;

            void push(
                const NbWindowHandle& handle, 
                const NbRect<int>& rect, 
                UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS
            ) noexcept;

            void apply() noexcept;
            

        private:
            HDWP hDefer = nullptr;
        };
    };

};


#endif