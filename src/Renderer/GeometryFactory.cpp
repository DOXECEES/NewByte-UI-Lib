
#include "GeometryFactory.hpp"

namespace Renderer
{
	NB_NODISCARD std::vector<NbPoint<float>> GeometryFactory::create(ShapeType type, const NbRect<int>& rect) noexcept
	{
		const auto& normalized = ShapeLibrary::get(type);

		std::vector<NbPoint<float>> result;
		result.reserve(normalized.size());

		for (auto& p : normalized)
		{
			result.push_back({
				rect.x + p.x * rect.width,
				rect.y + p.y * rect.height
				});
		}
		return result;
	}

	NB_NODISCARD const std::vector<NbPoint<float>>& ShapeLibrary::get(ShapeType type) noexcept
	{
		static const std::unordered_map<ShapeType, std::vector<NbPoint<float>>> shapes =
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



