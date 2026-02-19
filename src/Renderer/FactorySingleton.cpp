// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#include "FactorySingleton.hpp"

namespace Renderer
{
    Microsoft::WRL::ComPtr<ID2D1Factory1> FactorySingleton::getFactory() noexcept
    {
        static Microsoft::WRL::ComPtr<ID2D1Factory1> pFactory = nullptr;
        if (!pFactory)
        {
            D2D1_FACTORY_OPTIONS options = {};
            options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
            D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &options, &pFactory);                
        }
        return pFactory;
    }


    void FactorySingleton::releaseFactory() noexcept
    {
        Microsoft::WRL::ComPtr<ID2D1Factory> pFactory = getFactory();
        if (pFactory) pFactory->Release();
    }


    Microsoft::WRL::ComPtr<IDWriteFactory> FactorySingleton::getDirectWriteFactory() noexcept
    {
        static Microsoft::WRL::ComPtr<IDWriteFactory> writeFactory = nullptr;
        if (!writeFactory)
        {
            HRESULT hr = DWriteCreateFactory(
                DWRITE_FACTORY_TYPE_SHARED,
                __uuidof(IDWriteFactory),
                reinterpret_cast<IUnknown**>(writeFactory.GetAddressOf())
            );

            if (hr != S_OK)
            {
                return nullptr;
            }
        }

        return writeFactory;
    }


    ID2D1PathGeometry* FactorySingleton::getPathGeometry() noexcept
    {
        static ID2D1PathGeometry* pathGeometry = nullptr;
        if (!pathGeometry)
        {
            Microsoft::WRL::ComPtr<ID2D1Factory> factory = getFactory();

            HRESULT hr = factory->CreatePathGeometry(&pathGeometry);

            if (hr != S_OK)
            {
                return nullptr;
            }
        }

        return pathGeometry;
    }

    Microsoft::WRL::ComPtr<ID2D1PathGeometry> 
    FactorySingleton::createGeometry(nbstl::Span<D2D1_POINT_2F> verticies) noexcept
    {
        using namespace Microsoft::WRL;

        auto factory = FactorySingleton::getFactory();

        ComPtr<ID2D1PathGeometry> geometry;
        factory->CreatePathGeometry(&geometry);

        ComPtr<ID2D1GeometrySink> sink;
        geometry->Open(&sink);

        if (verticies.empty())
        {
            return geometry;
        }

        sink->BeginFigure(verticies[0], D2D1_FIGURE_BEGIN_FILLED);

        for (auto it = verticies.begin() + 1; it != verticies.end(); ++it)
        {
            sink->AddLine(*it);
        }

        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        sink->Close();

        return geometry;
    }
};
