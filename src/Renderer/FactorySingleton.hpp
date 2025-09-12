#ifndef NBUI_SRC_RENDERER_FACTORYSINGLETON_HPP
#define NBUI_SRC_RENDERER_FACTORYSINGLETON_HPP


#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include <dwrite.h>
#pragma comment(lib, "dwrite")

namespace Renderer
{
    class FactorySingleton
    {
        public:
            static ID2D1Factory* getFactory() noexcept;
            
            static void releaseFactory() noexcept;

            static IDWriteFactory* getDirectWriteFactory() noexcept;
            
            static ID2D1PathGeometry* getPathGeometry() noexcept;

        private:
            FactorySingleton() = default;
    };
};

#endif

