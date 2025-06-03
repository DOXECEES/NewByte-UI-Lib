#ifndef SRC_CORE_HPP
#define SRC_CORE_HPP


#include <Windows.h>
#include <cstdint>

#define DCX_USESTYLE 0x00010000

template <typename T>
struct NbPoint
{
    T x = {};
    T y = {};

    auto operator==(const NbPoint<T>& other) const { return x == other.x && y == other.y; }

    NbPoint<T> scale(double scale)
    {
        return NbPoint<T>(static_cast<int>((double)x * scale), static_cast<int>((double)y * scale));
    }
};

template <typename T>
struct NbSize
{
    T width  = {};
    T height = {};

    NbSize<T> scale(double scale)
    {
        return NbSize<T>(static_cast<T>((double)width * scale), static_cast<T>((double)height * scale));
    }
};


struct NbColor
{
    uint8_t r = {};
    uint8_t g = {};
    uint8_t b = {};
    uint8_t a = {};

    constexpr NbColor() = default;
    constexpr NbColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255) 
        : r(red), g(green), b(blue), a(alpha) {}


    operator COLORREF()
    {
        return RGB(r, g, b);
    }

    // связать с движком

};


template<typename T>
void SafeRelease(T** ppT) {
    if (*ppT) {
        (*ppT)->Release();
        *ppT = nullptr;
    }
}

/// @brief //
struct MouseButtons
{
    bool isLmbHolds;
};
/// @brief //


#endif