// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "BorderGeometryBuilder.hpp"

namespace Renderer
{
	namespace Geometry
	{
        BorderGeometryBuilder::GrooveBorderMesh BorderGeometryBuilder::buildGrooveMesh(const NbRect<int>& rect, int width) noexcept
        {
            GrooveBorderMesh mesh;

            int half = width / 2;

            NbRect<int> outer = rect.expand(width);
            NbRect<int> inner = rect.expand(half);

            {
                nbstl::Array<D2D1_POINT_2F, 6> upper;
                upper[0] = Direct2dUtils::toD2D1Point(outer.getBottomLeft());
                upper[1] = Direct2dUtils::toD2D1Point(outer.getTopLeft());
                upper[2] = Direct2dUtils::toD2D1Point(outer.getTopRight());
                upper[3] = Direct2dUtils::toD2D1Point(inner.getTopRight());
                upper[4] = Direct2dUtils::toD2D1Point(inner.getTopLeft());
                upper[5] = Direct2dUtils::toD2D1Point(inner.getBottomLeft());
                mesh.outerUpper = upper;

                nbstl::Array<D2D1_POINT_2F, 6> lower;
                lower[0] = Direct2dUtils::toD2D1Point(outer.getBottomLeft());
                lower[1] = Direct2dUtils::toD2D1Point(outer.getBottomRight());
                lower[2] = Direct2dUtils::toD2D1Point(outer.getTopRight());
                lower[3] = Direct2dUtils::toD2D1Point(inner.getTopRight());
                lower[4] = Direct2dUtils::toD2D1Point(inner.getBottomRight());
                lower[5] = Direct2dUtils::toD2D1Point(inner.getBottomLeft());
                mesh.outerLower = lower;
            }

            {
                NbRect<int> inner2 = rect;

                nbstl::Array<D2D1_POINT_2F, 6> upper;
                upper[0] = Direct2dUtils::toD2D1Point(inner.getBottomLeft());
                upper[1] = Direct2dUtils::toD2D1Point(inner.getTopLeft());
                upper[2] = Direct2dUtils::toD2D1Point(inner.getTopRight());
                upper[3] = Direct2dUtils::toD2D1Point(inner2.getTopRight());
                upper[4] = Direct2dUtils::toD2D1Point(inner2.getTopLeft());
                upper[5] = Direct2dUtils::toD2D1Point(inner2.getBottomLeft());
                mesh.innerUpper = upper;

                nbstl::Array<D2D1_POINT_2F, 6> lower;
                lower[0] = Direct2dUtils::toD2D1Point(inner.getBottomLeft());
                lower[1] = Direct2dUtils::toD2D1Point(inner.getBottomRight());
                lower[2] = Direct2dUtils::toD2D1Point(inner.getTopRight());
                lower[3] = Direct2dUtils::toD2D1Point(inner2.getTopRight());
                lower[4] = Direct2dUtils::toD2D1Point(inner2.getBottomRight());
                lower[5] = Direct2dUtils::toD2D1Point(inner2.getBottomLeft());
                mesh.innerLower = lower;
            }

            return mesh;
        }

        BorderGeometryBuilder::InsetBorderMesh BorderGeometryBuilder::buildInsetMesh(const NbRect<int>& rect, int width) noexcept
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

