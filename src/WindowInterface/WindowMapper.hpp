#ifndef SRC_WINDOWINTERFACE_WINDOWMAPPER_HPP
#define SRC_WINDOWINTERFACE_WINDOWMAPPER_HPP

#include "../Core.hpp"

#include <unordered_map>


#include "IWindow.hpp"

namespace WindowInterface
{
    class WindowMapper
    {
    public:
        static void registerWindow(const NbWindowHandle& handle, IWindow* window)
        { 
            windowMap[handle] = window;
        }

        static IWindow* getWindowByHandle(const NbWindowHandle& handle)
        {
            if(windowMap.find(handle) == windowMap.end()) return nullptr;
            return windowMap[handle];
        }

        static void unregisterWindow(const NbWindowHandle& handle)
        {
            windowMap.erase(handle);
        }

    private:
        inline static std::unordered_map<NbWindowHandle, IWindow*> windowMap;
    };
};

#endif