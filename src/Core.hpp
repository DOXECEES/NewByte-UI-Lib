#ifndef NBUI_SRC_CORE_HPP
#define NBUI_SRC_CORE_HPP

#include <cstdint>
#include <string>
#include <variant>
#include <stdexcept>
#include <cmath>
#include <sstream>
#include <ostream>
#define DCX_USESTYLE 0x00010000


template<typename T>
inline std::string toStdString(const T& val) noexcept
{
	std::ostringstream oss;
	oss << val;
	return oss.str();
}

enum class SpecialKeyCode
{
    NONE,
    CTRL,

};

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

    constexpr NbPoint<T> operator+(const NbPoint<T>& oth) const 
    {
        return NbPoint<T>(x + oth.x, y + oth.y); 
    }

    constexpr NbPoint<T> operator-(const NbPoint<T>& oth) const 
    {
        return NbPoint<T>(x - oth.x, y - oth.y); 
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

    NbSize<T>& operator-=(const NbSize<T>& oth)
    {
        this->width -= oth.width;
        this->height -= oth.height;
        return *this;
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
    constexpr NbRect() = default;

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

    constexpr bool isEmpty() const { return std::fabs(width) < std::numeric_limits<float>::epsilon()
        || std::fabs(height) < std::numeric_limits<float>::epsilon(); }
    constexpr void scale(const float scaleFactor) 
    {
        x *= scaleFactor;
        y *= scaleFactor;
        width *= scaleFactor;
        height *= scaleFactor;
    }

    constexpr void scaleX(const float scaleFactor) noexcept
    {
        x *= scaleFactor;
        width *= scaleFactor;
    }

    constexpr void scaleY(const float scaleFactor) noexcept
    {
        y *= scaleFactor;
        height *= scaleFactor;
    }

    constexpr void scale(const NbSize<float>& scaleFactor)
    {
        x *= scaleFactor.width;
        x = trunc(x);
        y *= scaleFactor.height;
        y = ceil(y);
        width *= scaleFactor.width;
        height *= scaleFactor.height;
    }

    constexpr NbRect<float> toFloat() const
    {
        return NbRect<float>(
            static_cast<float>(this->x),
            static_cast<float>(this->y),
            static_cast<float>(this->width),
            static_cast<float>(this->height)
        );
    }

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