#ifndef NBUI_SRC_RENDERER_FACTORYSINGLETON_HPP
#define NBUI_SRC_RENDERER_FACTORYSINGLETON_HPP


#include <NbCore.hpp>

#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include <dwrite.h>
#pragma comment(lib, "dwrite")
#include <wincodec.h> 
#pragma comment(lib, "windowscodecs.lib")  

#include <wrl.h>

#include <Core.hpp>
#include <Span.hpp>

#include <d2d1_1.h>


namespace Renderer
{
    class FactorySingleton
    {
        public:
            static Microsoft::WRL::ComPtr<ID2D1Factory1> getFactory() noexcept;
            
            static void releaseFactory() noexcept;

            static Microsoft::WRL::ComPtr<IDWriteFactory> getDirectWriteFactory() noexcept;
            
            static ID2D1PathGeometry* getPathGeometry() noexcept;

            static NB_NODISCARD Microsoft::WRL::ComPtr<ID2D1PathGeometry> createGeometry(nbstl::Span<D2D1_POINT_2F> verticies) noexcept;

            static NB_NODISCARD Microsoft::WRL::ComPtr<IWICImagingFactory> getWicFactory() noexcept;

        private:
            FactorySingleton() = default;
    };
};

#endif

