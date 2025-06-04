#ifndef SRC_RENDERER_RENDERER_HPP
#define SRC_RENDERER_RENDERER_HPP

#include "../Core.hpp"

#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include "FactorySingleton.hpp"

namespace Renderer
{
    class Renderer
    {
        public:
            Renderer() = default;
            ID2D1HwndRenderTarget* createHwndRenderTarget(HWND hwnd, const NbSize<int>& size) const noexcept;
            ID2D1SolidColorBrush* createSolidColorBrush(ID2D1HwndRenderTarget* renderTarget, const NbColor& color) const noexcept;


        private:
            ID2D1Factory* pFactory = FactorySingleton::getFactory();
    };
};

#endif