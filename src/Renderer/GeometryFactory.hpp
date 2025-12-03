#ifndef NBUI_SRC_RENDERER_GEOMETRYFACTORY_HPP
#define NBUI_SRC_RENDERER_GEOMETRYFACTORY_HPP

#include "NbCore.hpp"
#include "Core.hpp"

#include <vector>
#include <unordered_map>

namespace Renderer
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
		NB_NODISCARD static const std::vector<NbPoint<float>>& get(ShapeType type) noexcept;
	};

	class GeometryFactory
	{
	public:
		NB_NODISCARD static std::vector<NbPoint<float>> create(ShapeType type, const NbRect<int>& rect) noexcept;
	};

};

#endif
