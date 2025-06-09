#ifndef SRC_WIN32WINDOW_WIN32EVENTLOOP_HPP
#define SRC_WIN32WINDOW_WIN32EVENTLOOP_HPP

#include <Windows.h>

#include "../WindowInterface/IEventLoop.hpp"


namespace Win32Window
{

    class WindowMessage : public WindowInterface::IPlatformMessage
    {
    public:
        WindowMessage() = delete;
        WindowMessage(const MSG& msg) : IPlatformMessage(), msg(msg) {}
        ~WindowMessage() noexcept = default;

        virtual std::any getNativeMessage() const override { return msg; }
        const MSG& getMsg() const { return msg; }

    private:
        MSG msg;
    };

    class Win32EventLoop : public WindowInterface::IEventLoop
    {
    public:
        Win32EventLoop() {};
        ~Win32EventLoop() {};

        virtual WindowInterface::IPlatformMessage* getNextMessage() override;
        //virtual void stop() override;

        //virtual bool isRunning() override;
        virtual void dispatchEvent(const WindowInterface::IPlatformMessage& message) override;
    
    private:
        WindowMessage lastMessage = {{}};
    };
}

#endif