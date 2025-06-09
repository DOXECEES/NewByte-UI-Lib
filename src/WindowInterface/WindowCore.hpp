#ifndef SRC_WINDOWINTERFACE_WINDOWCORE_HPP
#define SRC_WINDOWINTERFACE_WINDOWCORE_HPP

#include <variant>
#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <functional>

class NbWindowHandle
{
public:
    enum class PlatformType
    {
        UNKNOWN,
        WIN,
        X11
    };

    NbWindowHandle() = default;

    bool isValid() const noexcept { return platform != PlatformType::UNKNOWN; }
    PlatformType getPlatform() const noexcept { return platform; }

    // API: из Windows HWND
    template <typename T>
    static NbWindowHandle fromWinHandle(T hwnd)
    {
        static_assert(std::is_pointer_v<T>, "fromWinHandle expects pointer");
        return NbWindowHandle(reinterpret_cast<void *>(hwnd), PlatformType::WIN);
    }

    static NbWindowHandle fromX11Handle(int xid)
    {
        return NbWindowHandle(xid, PlatformType::X11);
    }

    static NbWindowHandle fromX11HandlePair(void *display, unsigned long window)
    {
        return NbWindowHandle(std::make_pair(display, window), PlatformType::X11);
    }

    template <typename T>
    T as() const
    {
        if constexpr (std::is_same_v<T, void *>)
        {
            if (!std::holds_alternative<void *>(handle))
            {
                throw std::runtime_error("Invalid handle access (void*)");
            }
            return std::get<void *>(handle);
        }
        else if constexpr (std::is_pointer_v<T>)
        {
            if (!std::holds_alternative<void *>(handle))
            {
                throw std::runtime_error("Invalid handle access (ptr)");
            }
            return reinterpret_cast<T>(std::get<void *>(handle));
        }
        else if constexpr (std::is_same_v<T, int>)
        {
            if (!std::holds_alternative<int>(handle))
            {
                throw std::runtime_error("Invalid handle access (int)");
            }
            return std::get<int>(handle);
        }
        else if constexpr (std::is_same_v<T, std::pair<void *, unsigned long>>)
        {
            if (!std::holds_alternative<std::pair<void *, unsigned long>>(handle))
            {
                throw std::runtime_error("Invalid handle access (x11 pair)");
            }
            return std::get<std::pair<void *, unsigned long>>(handle);
        }
        else
        {
            static_assert(sizeof(T) == 0, "Unsupported type for NbWindowHandle::as<T>()");
        }
    }

    bool operator==(const NbWindowHandle &other) const noexcept
    {
        return platform == other.platform && handle == other.handle;
    }

private:
    PlatformType platform = PlatformType::UNKNOWN;
    std::variant<std::monostate, void *, int, std::pair<void *, unsigned long>> handle;

    NbWindowHandle(void *ptr, PlatformType p) : platform(p), handle(ptr) {}
    NbWindowHandle(int val, PlatformType p) : platform(p), handle(val) {}
    NbWindowHandle(std::pair<void *, unsigned long> val, PlatformType p) : platform(p), handle(val) {}

    friend struct std::hash<NbWindowHandle>;
};

namespace std
{
    template <>
    struct hash<NbWindowHandle>
    {
        size_t operator()(const NbWindowHandle &nh) const noexcept
        {
            size_t h1 = std::hash<int>()(static_cast<int>(nh.getPlatform()));

            size_t h2 = std::visit([](const auto &val) -> size_t
                                   {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::monostate>) {
                    return 0;
                } else if constexpr (std::is_same_v<T, void*>) {
                    return std::hash<std::uintptr_t>()(reinterpret_cast<std::uintptr_t>(val));
                } else if constexpr (std::is_same_v<T, int>) {
                    return std::hash<int>()(val);
                } else if constexpr (std::is_same_v<T, std::pair<void*, unsigned long>>) {
                    size_t h1 = std::hash<std::uintptr_t>()(reinterpret_cast<std::uintptr_t>(val.first));
                    size_t h2 = std::hash<unsigned long>()(val.second);
                    return h1 ^ (h2 << 1);
                } else {
                    return 0;
                } }, nh.handle);

            return h1 ^ (h2 << 1);
        }
    };
}

#endif