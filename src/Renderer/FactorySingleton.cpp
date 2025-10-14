// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#include "FactorySingleton.hpp"

namespace Renderer
{
    Microsoft::WRL::ComPtr<ID2D1Factory> FactorySingleton::getFactory() noexcept
    {
        static Microsoft::WRL::ComPtr<ID2D1Factory> pFactory = nullptr;
        if (!pFactory)
        {
            D2D1_FACTORY_OPTIONS options = {};
            options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
            D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, pFactory.GetAddressOf());
                
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
        Microsoft::WRL::ComPtr<ID2D1Factory> factory = getFactory();
        static ID2D1PathGeometry* pathGeometry = nullptr;
        if (!pathGeometry)
        {
            HRESULT hr = factory->CreatePathGeometry(&pathGeometry);

            if (hr != S_OK)
            {
                return nullptr;
            }
        }

        return pathGeometry;
    }
};
