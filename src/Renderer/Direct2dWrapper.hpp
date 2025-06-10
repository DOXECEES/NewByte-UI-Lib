#ifndef SRC_RENDERER_DIRECT2DWRAPPER_HPP
#define SRC_RENDERER_DIRECT2DWRAPPER_HPP

#include "../Core.hpp"
#include "../WindowInterface/WindowCore.hpp"
#include "FactorySingleton.hpp"
#include "../Utils.hpp"

#include <unordered_map>

#include <d2d1.h>
#pragma comment(lib, "d2d1")

namespace Direct2dUtils
{
    D2D1_RECT_F toD2D1Rect(const NbRect<int>& rect) noexcept;
}

class Direct2dHandleRenderTarget
{
public:
    Direct2dHandleRenderTarget(ID2D1HwndRenderTarget *renderTarget)
    {
        this->renderTarget = renderTarget;
    }

    ~Direct2dHandleRenderTarget()
    {
        for (auto &color : colorMap)
        {
            SafeRelease(&color.second);
        }
        SafeRelease(&renderTarget);
    }

    void resize(const NbSize<int> &size) noexcept
    {
        renderTarget->Resize(D2D1::SizeU(size.width, size.height));
    }

    void drawRectangle(const NbRect<int>& rect, const NbColor& color, const float strokeWidth = 1.0f) const noexcept
    {
        ID2D1SolidColorBrush *brush = createSolidBrush(color);
        renderTarget->DrawRectangle(Direct2dUtils::toD2D1Rect(rect), brush, strokeWidth);
    }

    void fillRectangle(const NbRect<int>& rect, const NbColor& color) const noexcept
    {
        ID2D1SolidColorBrush *brush = createSolidBrush(color);
        renderTarget->FillRectangle(Direct2dUtils::toD2D1Rect(rect), brush);
    }

    ID2D1SolidColorBrush *createSolidBrush(const NbColor &color) const noexcept
    {
        if (colorMap.find(color) != colorMap.end())
        {
            return colorMap.at(color);
        }

        ID2D1SolidColorBrush *brush = nullptr;
        renderTarget->CreateSolidColorBrush(Utils::toD2D1Color(color), &brush);
        colorMap[color] = brush;
        return brush;
    }

    ID2D1HwndRenderTarget *getRawRenderTarget() const noexcept
    {
        return renderTarget;
    }

private:
    ID2D1HwndRenderTarget *renderTarget = nullptr;
    mutable std::unordered_map<NbColor, ID2D1SolidColorBrush *> colorMap;
};


class Direct2dWrapper
{

public:
    static Direct2dHandleRenderTarget createRenderTarget(const NbWindowHandle &handle, const NbSize<int> &size) noexcept;
    static ID2D1SolidColorBrush* createSolidColorBrush(const Direct2dHandleRenderTarget &renderTarget, const NbColor &color) noexcept;

private:
    inline static ID2D1Factory* factory = Renderer::FactorySingleton::getFactory();
};

#endif