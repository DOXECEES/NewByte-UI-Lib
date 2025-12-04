#ifndef NBUI_SRC_RENDERER_GEOMETRY_BORDERGEOMETRYBUILDER_HPP
#define NBUI_SRC_RENDERER_GEOMETRY_BORDERGEOMETRYBUILDER_HPP

#include <Alghorithm.hpp>
#include <Array.hpp>

#include <d2d1.h>
#include "Renderer/Direct2dWrapper.hpp"

namespace Renderer
{
	namespace Geometry
	{
		class BorderGeometryBuilder
		{
        public:
            struct InsetBorderMesh
            {
                nbstl::Array<D2D1_POINT_2F, 6> upper;
                nbstl::Array<D2D1_POINT_2F, 6> lower;
            };

            static InsetBorderMesh buildInsetMesh(const NbRect<int>& rect, int width)
            {
                InsetBorderMesh mesh;

                NbRect<int> expandedRect = rect.expand(width);
                
                nbstl::Array<D2D1_POINT_2F, 6> upper;
                upper[0] = Direct2dUtils::toD2D1Point(expandedRect.getBottomLeft());
                upper[1] = Direct2dUtils::toD2D1Point(expandedRect.getTopLeft());
                upper[2] = Direct2dUtils::toD2D1Point(expandedRect.getTopRight());
                upper[3] = Direct2dUtils::toD2D1Point(rect.getTopRight());
                upper[4] = Direct2dUtils::toD2D1Point(rect.getTopLeft());
                upper[5] = Direct2dUtils::toD2D1Point(rect.getBottomLeft());
                mesh.upper = upper;

                nbstl::Array<D2D1_POINT_2F, 6> lower;
                lower[0] = Direct2dUtils::toD2D1Point(expandedRect.getBottomLeft());
                lower[1] = Direct2dUtils::toD2D1Point(expandedRect.getBottomRight());
                lower[2] = Direct2dUtils::toD2D1Point(expandedRect.getTopRight());
                lower[3] = Direct2dUtils::toD2D1Point(rect.getTopRight());
                lower[4] = Direct2dUtils::toD2D1Point(rect.getBottomRight());
                lower[5] = Direct2dUtils::toD2D1Point(rect.getBottomLeft());
                mesh.lower = lower;

                return mesh;
            }
		};
	};
};

#endif