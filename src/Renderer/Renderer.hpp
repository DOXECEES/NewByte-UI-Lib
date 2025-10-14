#ifndef NBUI_SRC_RENDERER_RENDERER_HPP
#define NBUI_SRC_RENDERER_RENDERER_HPP

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
            ~Renderer();
            ID2D1HwndRenderTarget* createHwndRenderTarget(HWND hwnd, const NbSize<int>& size) const noexcept;


        private:
            Microsoft::WRL::ComPtr<ID2D1Factory> pFactory = FactorySingleton::getFactory();
    };
};

#endif