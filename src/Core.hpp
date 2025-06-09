#ifndef SRC_CORE_HPP
#define SRC_CORE_HPP

#include <cstdint>
#include <string>
#include <variant>
#include <stdexcept>

#define DCX_USESTYLE 0x00010000

// get from boost::hash_combine
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}




template <typename T>
struct NbPoint
{
    constexpr NbPoint() = default;
    constexpr NbPoint(const T& x, const T& y) : x(x), y(y) {}

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

    constexpr NbSize() = default;
    constexpr NbSize(const T& width, const T& height) : width(width), height(height) {}

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



    bool operator==(const NbColor& other) const
    { 
        return r == other.r 
            && g == other.g
            && b == other.b 
            && a == other.a; 
    }

    // связать с движком
};

template<typename T>
struct NbRect
{

    NbRect(const T x, const T y, const T width, const T height)
    : x(x)
    , y(y)
    , width(width)
    , height(height) 
    {} 


    T x         = {};
    T y         = {};
    T width     = {};
    T height    = {};



};



template <>
struct std::hash<NbColor>
{
  std::size_t operator()(const NbColor& color) const
  {
    using std::size_t;
    using std::hash;
    using std::string;


    size_t seed = 0;
    hash_combine(seed, color.r);
    hash_combine(seed, color.g);
    hash_combine(seed, color.b);
    hash_combine(seed, color.a);

    return seed;
  }
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