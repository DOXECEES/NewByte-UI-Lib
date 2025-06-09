#ifndef SRC_WINDOWINTERFACE_IEVENTLOOP_HPP
#define SRC_WINDOWINTERFACE_IEVENTLOOP_HPP

#include <optional>

#include "IWindow.hpp"

#include "IPlatformMessage.hpp"


namespace WindowInterface
{
    class IEventLoop
    {

    public:

        void run();
        //virtual void stop() = 0;
    
        virtual WindowInterface::IPlatformMessage* getNextMessage() = 0;

        //virtual bool isRunning() = 0;
        virtual void dispatchEvent(const IPlatformMessage& message) = 0;


        //virtual ~IEventLoop() = default;

    };
};

#endif