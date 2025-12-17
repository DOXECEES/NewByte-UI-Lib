
#include "GeometryFactory.hpp"

namespace Renderer::Geometry
{
	NB_NODISCARD nbstl::Vector<NbPoint<float>> GeometryFactory::create(ShapeType type, const NbRect<int>& rect) noexcept
	{
		const auto& normalized = ShapeLibrary::get(type);

		nbstl::Vector<NbPoint<float>> result;
		result.reserve(normalized.size());

		for (auto& p : normalized)
		{
			result.pushBack({
				rect.x + p.x * rect.width,
				rect.y + p.y * rect.height
				});
		}
		return result;
	}

	NB_NODISCARD const nbstl::Vector<NbPoint<float>>& ShapeLibrary::get(ShapeType type) noexcept
	{
		static const std::unordered_map<ShapeType, nbstl::Vector<NbPoint<float>>> shapes =
		{
			{ ShapeType::CHECK_MARK, {
				{  5.0f / 48.0f, 14.0f / 48.0f },
				{ 23.0f / 48.0f, 32.0f / 48.0f },
				{ 50.0f / 48.0f,  3.0f / 48.0f },
				{ 21.0f / 48.0f, 47.0f / 48.0f },
				{ -3.0f / 48.0f, 27.0f / 48.0f }
			}},
			{ ShapeType::CROSS, {
				{ 0.0f, 0.0f }, { 1.0f, 1.0f },
				{ 1.0f, 0.0f }, { 0.0f, 1.0f }
			}},
			{ ShapeType::ARROW, {
				{ 0.0f, 0.5f }, { 1.0f, 0.0f },
				{ 1.0f, 1.0f }
			}}
		};

		return shapes.at(type);
	}
};



