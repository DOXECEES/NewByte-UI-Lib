#ifndef NBUI_SRC_RENDERER_GEOMETRY_WIDGETSGEOMETRYBUILDER_HPP
#define NBUI_SRC_RENDERER_GEOMETRY_WIDGETSGEOMETRYBUILDER_HPP

#include "Core.hpp"
#include <wrl.h>
#include <d2d1.h>

namespace Renderer
{
    namespace Geometry
    {
        class WidgetsGeometryBuilder
        {
        public:
            WidgetsGeometryBuilder() noexcept = default;
            ~WidgetsGeometryBuilder() noexcept = default;

            Microsoft::WRL::ComPtr<ID2D1Geometry> createCheckBoxArrow(const NbRect<int>& rect) noexcept;

        };
    };
};
#endif 
