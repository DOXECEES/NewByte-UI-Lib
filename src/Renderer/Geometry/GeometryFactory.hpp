#ifndef NBUI_SRC_RENDERER_GEOMETRY_GEOMETRYFACTORY_HPP
#define NBUI_SRC_RENDERER_GEOMETRY_GEOMETRYFACTORY_HPP

#include "NbCore.hpp"
#include "Core.hpp"

#include <Vector.hpp>
#include <unordered_map>

namespace Renderer
{
	namespace Geometry
	{
		enum class ShapeType
		{
			CHECK_MARK,
			CROSS,
			ARROW,
		};

		class ShapeLibrary
		{
		public:
			NB_NODISCARD static const nbstl::Vector<NbPoint<float>>& get(ShapeType type) noexcept;
		};

		class GeometryFactory
		{
		public:
			NB_NODISCARD static nbstl::Vector<NbPoint<float>> create(ShapeType type, const NbRect<int>& rect) noexcept;
		};

	};
};

#endif
