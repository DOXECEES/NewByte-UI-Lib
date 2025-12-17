#include "BorderGeometryCache.hpp"

namespace Renderer
{
    namespace Geometry
    {
        const BorderGeometryCache::GeometrySet& 
        BorderGeometryCache::getMesh(const Key& key) noexcept
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

        BorderGeometryCache::GeometrySet 
        BorderGeometryCache::createGeometry(Border::Style style, const NbRect<int>& rect, float thickness) noexcept
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


    };
};