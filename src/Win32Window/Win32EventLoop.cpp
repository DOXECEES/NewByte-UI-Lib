#include "Win32EventLoop.hpp"

#include "../WindowInterface/WindowMapper.hpp"

namespace Win32Window
{

    WindowInterface::IPlatformMessage* Win32EventLoop::getNextMessage()
    {
        MSG msg = {};
        if(!GetMessage(&msg, nullptr, 0, 0) )
        {
            return nullptr;
        }

        lastMessage = WindowMessage(msg);
        return &lastMessage;
    }


    void Win32EventLoop::dispatchEvent(const WindowInterface::IPlatformMessage &message)
    {
        auto& winMsg = static_cast<const WindowMessage&>(message);

        ::TranslateMessage(&winMsg.getMsg());
        ::DispatchMessage(&winMsg.getMsg());

    }
};
