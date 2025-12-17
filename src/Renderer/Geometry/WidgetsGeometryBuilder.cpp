#include "WidgetsGeometryBuilder.hpp"

#include "GeometryFactory.hpp"
#include "Renderer/FactorySingleton.hpp"

namespace Renderer::Geometry 
{
    Microsoft::WRL::ComPtr<ID2D1Geometry> WidgetsGeometryBuilder::createCheckBoxArrow(const NbRect<int>& rect) noexcept
    {
        nbstl::Vector<NbPoint<float>> vertex = GeometryFactory::create(ShapeType::CHECK_MARK, rect);
        
		Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry;
		Microsoft::WRL::ComPtr<ID2D1GeometrySink> sink;
		Renderer::FactorySingleton::getFactory()->CreatePathGeometry(&geometry);

		HRESULT hr = geometry->Open(&sink);

		if (SUCCEEDED(hr))
		{
			sink->BeginFigure(
				D2D1::Point2F(vertex[0].x, vertex[0].y),
				D2D1_FIGURE_BEGIN_FILLED
			);

			for (size_t i = 1; i < vertex.size(); ++i)
			{
				sink->AddLine(D2D1::Point2F(vertex[i].x, vertex[i].y));
			}

			sink->EndFigure(D2D1_FIGURE_END_CLOSED);

			hr = sink->Close();
		}
		
		return geometry;
    }
}; 