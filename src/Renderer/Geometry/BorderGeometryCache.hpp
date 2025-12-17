#ifndef NBUI_SRC_RENDERER_GEOMETRY_BORDERGEOMETRYCACHE_HPP
#define NBUI_SRC_RENDERER_GEOMETRY_BORDERGEOMETRYCACHE_HPP

#include <Core.hpp>
#include <Array.hpp>    
#include <Vector.hpp>
#include <map>
#include <functional>

#include <wrl.h>
#include <d2d1.h>
#include "Widgets/WidgetStyle.hpp"
#include "BorderGeometryBuilder.hpp"
#include "Renderer/FactorySingleton.hpp"

namespace Renderer
{
    namespace Geometry
    {

        class BorderGeometryCache
        {
        public:

            struct GeometrySet
            {
                nbstl::Vector<Microsoft::WRL::ComPtr<ID2D1Geometry>> geometries;
            };

            struct Key
            {
                Border::Style style;
                NbRect<int> rect;
                int thickness;

                bool operator<(const Key& other) const
                {
                    return std::tie(style, rect.x, rect.y, rect.width, rect.height, thickness) <
                        std::tie(other.style, other.rect.x, other.rect.y, other.rect.width, other.rect.height, other.thickness);
                }
            };

            const GeometrySet& getMesh(const Key& key) noexcept;

            BorderGeometryCache() noexcept = default;
            ~BorderGeometryCache() noexcept = default;

        private:
            GeometrySet createGeometry(Border::Style style, const NbRect<int>& rect, float thickness) noexcept;

        private:

            std::map<Key, GeometrySet> cache;
        };
    };
};
#endif 