#ifndef SRC_RENDERER_FACTORYSINGLETON_HPP
#define SRC_RENDERER_FACTORYSINGLETON_HPP


#include <d2d1.h>
#pragma comment(lib, "d2d1")

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
                    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
                }
                return pFactory;
            }

            static void releaseFactory()
            {
                ID2D1Factory* pFactory = getFactory();
                if (pFactory) pFactory->Release();
            }
            
        private:


    };
};

#endif

