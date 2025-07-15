#ifndef NBUI_SRC_WINDOWINTERFACE_IPLATFORMMESSAGE_HPP
#define NBUI_SRC_WINDOWINTERFACE_IPLATFORMMESSAGE_HPP

#include <any>

namespace WindowInterface
{
    class IPlatformMessage
    {

    public:
        //virtual ~IPlatformMessage() noexcept = default;
        virtual std::any getNativeMessage() const = 0;
    };
};

#endif