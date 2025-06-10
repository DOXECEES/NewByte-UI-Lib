#ifndef SRC_RENDERER_FACTORYSINGLETON_HPP
#define SRC_RENDERER_FACTORYSINGLETON_HPP


#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include <dwrite.h>
#pragma comment(lib, "dwrite")

namespace Renderer
{
    class FactorySingleton
    {
        public:
            static ID2D1Factory* getFactory()
            {
                
                static ID2D1Factory* pFactory = nullptr;
                if (!pFactory)
                {
                    D2D1_FACTORY_OPTIONS options = {};
                    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
                    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, &pFactory);

                }
                return pFactory;
            }

            static void releaseFactory()
            {
                ID2D1Factory* pFactory = getFactory();
                if (pFactory) pFactory->Release();
            }

            static IDWriteFactory* getDirectWriteFactory()
            {
                static IDWriteFactory* writeFactory = nullptr;
                if (!writeFactory)
                {
                    HRESULT hr = DWriteCreateFactory(
                        DWRITE_FACTORY_TYPE_SHARED,
                        __uuidof(IDWriteFactory),
                        reinterpret_cast<IUnknown**>(&writeFactory)
                    );

                    if(hr != S_OK)
                    {
                        return nullptr;
                    }
                }

                return writeFactory;
            }
            
        private:
            FactorySingleton() = default;
    };
};

#endif

