#ifndef NBUI_SRC_RENDERER_GEOMETRY_WIDGETSGEOMETRYCACHE_HPP
#define NBUI_SRC_RENDERER_GEOMETRY_WIDGETSGEOMETRYCACHE_HPP

#include "Core.hpp"

#include "WidgetsGeometryBuilder.hpp"
#include "GeometryFactory.hpp"
#include <map>

#include <d2d1.h>
#include <wrl.h>

namespace Renderer
{
    namespace Geometry 
    {
        class WidgetsGeometryCache
        {
        public:
            WidgetsGeometryCache() = default;
            ~WidgetsGeometryCache() = default;

            struct Key
            {
                NbRect<int> rect;
                int index;

                bool operator<(const Key& other) const
                {
                    return std::tie(rect.x, rect.y, rect.width, rect.height, index) <
                        std::tie(other.rect.x, other.rect.y, other.rect.width, other.rect.height, other.index);
                }
            };

            Microsoft::WRL::ComPtr<ID2D1Geometry> getMesh(ShapeType type, const Key& key) noexcept
            {

                if (cache.find(key) != cache.end())
                {
                    return cache[key];
                }

                Microsoft::WRL::ComPtr<ID2D1Geometry> geometry;

                switch (type)
                {
                    case ShapeType::CHECK_MARK:
                    {
                        geometry = builder.createCheckBoxArrow(key.rect);
                        break;
                    }
                }

                cache[key] = geometry;

                return cache.at(key);

            }

        private:
            WidgetsGeometryBuilder builder;
            std::map<Key, Microsoft::WRL::ComPtr<ID2D1Geometry>> cache;
        };
    }; 
}
#endif 
