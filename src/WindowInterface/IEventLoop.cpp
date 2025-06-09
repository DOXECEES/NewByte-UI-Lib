#include "IEventLoop.hpp"

namespace WindowInterface
{
    void IEventLoop::run()
    {
        WindowInterface::IPlatformMessage *message = getNextMessage();
        while (message != nullptr)
        {
            dispatchEvent(*message);

            message = getNextMessage();
        }
    }

}