// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
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