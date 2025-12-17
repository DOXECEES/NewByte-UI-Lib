#ifndef NBUI_SRC_RENDERERINTERFACE_RENDERTYPES_HPP
#define NBUI_SRC_RENDERERINTERFACE_RENDERTYPES_HPP

#include <Types.hpp>

namespace Renderer 
{
    using BrushHandle       = uint64;
    using BitmapHandle      = uint64;
    using FontHandle        = uint64;
    using TextFormatHandle  = uint64;
    using GeometryHandle    = uint64;

    constexpr BrushHandle INVALID_BRUSH_HANDLE = 0;
    constexpr BitmapHandle INVALID_BITMAP_HANDLE = 0;
    constexpr FontHandle INVALID_FONT_HANDLE = 0;
    constexpr TextFormatHandle INVALID_TEXT_FORMAT_HANDLE = 0;
    constexpr GeometryHandle INVALID_GEOMETRY_HANDLE = 0;


    enum class AntialiasMode : uint8_t
    {
        PerPrimitive,   // Сглаживание для каждого примитива
        Aliased         // Без сглаживания
    };

    enum class FillMode : uint8_t 
    {
        Alternate,      // Чередующееся заполнение
        Winding         // По правилу ненулевого индекса
    };

}; 

#endif 
