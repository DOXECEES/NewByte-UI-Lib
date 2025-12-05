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

            const GeometrySet&  
            getMesh(const Key& key)
            {
                auto it = cache.find(key);
                if (it != cache.end())
                {
                    return it->second;
                }

                GeometrySet geom = createGeometry(key.style, key.rect, key.thickness);
                Debug::debug(geom.geometries.at(0).Get());

                cache[key] = geom;
                Debug::debug(cache[key].geometries.at(0).Get());

                return cache.at(key);
            }


            BorderGeometryCache() noexcept = default;
            ~BorderGeometryCache() noexcept = default;
        
        private:
            GeometrySet
            createGeometry(Border::Style style, const NbRect<int>& rect, float thickness)
            {
                GeometrySet set;

                switch (style)
                {
                case Border::Style::OUTSET:
                case Border::Style::INSET:
                {
                    auto mesh = Geometry::BorderGeometryBuilder::buildInsetMesh(rect, thickness);
                    set.geometries.pushBack(FactorySingleton::createGeometry(mesh.upper));
                    set.geometries.pushBack(FactorySingleton::createGeometry(mesh.lower));

                    break;
                }
                case Border::Style::GROOVE:
                {
                    Geometry::BorderGeometryBuilder::GrooveBorderMesh mesh = Geometry::BorderGeometryBuilder::buildGrooveMesh(rect, thickness);
                    set.geometries.pushBack(FactorySingleton::createGeometry(mesh.innerLower));
                    set.geometries.pushBack(FactorySingleton::createGeometry(mesh.innerUpper));
                    set.geometries.pushBack(FactorySingleton::createGeometry(mesh.outerLower));
                    set.geometries.pushBack(FactorySingleton::createGeometry(mesh.outerUpper));

                    break;
                }
               /* case Border::Style::OUTSET:
                {
                    auto mesh = Geometry::BorderGeometryBuilder::buildOutsetMesh(rect, thickness);
                    set.geometries.push_back(Factory::createGeometry(mesh.upper));
                    set.geometries.push_back(Factory::createGeometry(mesh.lower));
                    set.colors.push_back(color);
                    set.colors.push_back(color.addMask(55));
                    break;
                }
                case Border::Style::SOLID:
                {
                    set.geometries.push_back(Factory::createGeometry(rect));
                    set.colors.push_back(color);
                    break;
                }
                case Border::Style::DASHED:
                {
                    auto dashedMesh = buildDashedMesh(size, thickness);
                    set.geometries.push_back(Factory::createGeometry(dashedMesh));
                    set.colors.push_back(color);
                    break;
                }
                default:
                    break;*/
                }
                return set;
            }


        private:


            std::map<Key, GeometrySet> cache;
        };
    };
};
#endif 