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

            struct GrooveBorderMesh
            {
                nbstl::Array<D2D1_POINT_2F, 6> innerUpper;
                nbstl::Array<D2D1_POINT_2F, 6> innerLower;
                nbstl::Array<D2D1_POINT_2F, 6> outerUpper;
                nbstl::Array<D2D1_POINT_2F, 6> outerLower;
            };

            static GrooveBorderMesh buildGrooveMesh(const NbRect<int>& rect, int width) noexcept;
           
            static InsetBorderMesh buildInsetMesh(const NbRect<int>& rect, int width) noexcept;
            
		};
	};
};

#endif