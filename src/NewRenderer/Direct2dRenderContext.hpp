#ifndef NBUI_SRC_NEWRENDERER_DIRECT2DRENDERCONTEXT_HPP
#define NBUI_SRC_NEWRENDERER_DIRECT2DRENDERCONTEXT_HPP

#include <NbCore.hpp>
#include <NonOwningPtr.hpp>
#include <d3d11.h>
#include <d2d1_1.h>

#include <d2d1_2.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <dxgi1_2.h>
#include <stack>
#include "RendererInterface/IRenderContext.hpp"
#include "RendererInterface/IResourceManager.hpp"

namespace Renderer
{
    // -------------------------
    // Утилиты для преобразований типов Nb -> Direct2D
    // -------------------------
    struct Direct2DUtils
    {
        static D2D1_RECT_F toD2DRect(const NbRect<int>& rect) noexcept
        {
            return D2D1::RectF(static_cast<float>(rect.x), static_cast<float>(rect.y), static_cast<float>(rect.x + rect.width), static_cast<float>(rect.y + rect.height));
        }

        static D2D1_COLOR_F toD2DColor(const NbColor& color) noexcept
        {
            return D2D1::ColorF(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
        }

        static D2D1_POINT_2F toD2DPoint(const NbPoint<int>& point) noexcept
        {
            return D2D1::Point2F(static_cast<float>(point.x), static_cast<float>(point.y));
        }

        static D2D1_RECT_F toD2DRect(const NbRect<float>& rect) noexcept
        {
            return D2D1::RectF(static_cast<float>(rect.x), static_cast<float>(rect.y), static_cast<float>(rect.x + rect.width), static_cast<float>(rect.y + rect.height));
        }

        static D2D1_POINT_2F toD2DPoint(const NbPoint<float>& point) noexcept
        {
            return D2D1::Point2F(static_cast<float>(point.x), static_cast<float>(point.y));
        }

    };

    // -------------------------
    // Сам контекст рендеринга
    // -------------------------
    class Direct2DRenderContext final : public IRenderContext
    {
    public:
        explicit Direct2DRenderContext(HWND hwnd, IResourceManager* resourceManager);
        ~Direct2DRenderContext() override;

        NB_NON_COPYABLE(Direct2DRenderContext);
        Direct2DRenderContext(Direct2DRenderContext&& other) noexcept;
        Direct2DRenderContext& operator=(Direct2DRenderContext&& other) noexcept;

        NB_NODISCARD bool initialize() noexcept;
        NB_NODISCARD bool resize(uint32_t width, uint32_t height) noexcept;


        void setResourceManager(IResourceManager* mgr) { resourceManager = mgr; }
        // === IRenderContext implementation ===
        void beginDraw() noexcept override;
        NB_NODISCARD bool endDraw() noexcept override;
        void clear(const NbColor& color) noexcept override;
        void flush() noexcept override;

        void drawRectangle(const NbRect<int>& rect, BrushHandle brush) noexcept override;
        void fillRectangle(const NbRect<int>& rect, BrushHandle brush) noexcept override;
        void drawRoundedRectangle(const NbRect<int>& rect, float radius, BrushHandle brush) noexcept override;
        void fillRoundedRectangle(const NbRect<int>& rect, float radius, BrushHandle brush) noexcept override;
        void drawEllipse(const NbPoint<int>& center, float radiusX, float radiusY, BrushHandle brush) noexcept override;
        void fillEllipse(const NbPoint<int>& center, float radiusX, float radiusY, BrushHandle brush) noexcept override;
        void drawLine(const NbPoint<int>& start, const NbPoint<int>& end, BrushHandle brush, float strokeWidth = 1.0f) noexcept override;
        void drawText(const std::wstring& text, const NbRect<int>& layoutRect, TextFormatHandle format, BrushHandle brush) noexcept override;
        void drawBitmap(const NbRect<int>& destination, BitmapHandle bitmap, float opacity = 1.0f) noexcept override;
        void drawBitmap(const NbRect<int>& destination, const NbRect<int>& sourceRect, BitmapHandle bitmap, float opacity = 1.0f) noexcept override;

        void pushClip(const NbRect<int>& clipRect) noexcept override;
        void popClip() noexcept override;
        void pushLayer(const NbRect<int>& bounds, float opacity = 1.0f) noexcept override;
        void popLayer() noexcept override;

        void setAntialiasMode(AntialiasMode mode) noexcept override;
        NB_NODISCARD AntialiasMode getAntialiasMode() const noexcept override;

        NB_NODISCARD float getDpi() const noexcept override;
        NB_NODISCARD NbSize<int> getSize() const noexcept override;
        NB_NODISCARD bool isValid() const noexcept override;

        // Direct2D-специфичные методы
        NB_NODISCARD ID2D1DeviceContext* getNativeDeviceContext() const noexcept;
        NB_NODISCARD ID2D1Factory2* getNativeFactory() const noexcept;

    private:
        NB_NODISCARD bool createDeviceResources() noexcept;
        bool createTargetBitmap() noexcept;

        void releaseDeviceResources() noexcept;

        NB_NODISCARD ID2D1Brush* getNativeBrush(BrushHandle handle) const noexcept;
        NB_NODISCARD IDWriteTextFormat* getNativeTextFormat(TextFormatHandle handle) const noexcept;
        NB_NODISCARD ID2D1Bitmap1* getNativeBitmap(BitmapHandle handle) const noexcept;

    private:
        HWND hwnd;
        IResourceManager* resourceManager;

        nbstl::NonOwningPtr<ID2D1Factory2> d2dFactory;
        nbstl::NonOwningPtr<IDWriteFactory> dwriteFactory;
        nbstl::NonOwningPtr<IWICImagingFactory> wicFactory;

        Microsoft::WRL::ComPtr<ID2D1Device> d2dDevice;
        Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2dContext;
        Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
        Microsoft::WRL::ComPtr<ID2D1Bitmap1> targetBitmap;
        Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3dContext;
        Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;


        std::stack<D2D1_MATRIX_3X2_F> transformStack;
        std::stack<D2D1_RECT_F> clipStack;
        std::stack<Microsoft::WRL::ComPtr<ID2D1Layer>> layerStack;

        float dpi{ 96.0f };
        D2D1_ANTIALIAS_MODE antialiasMode{ D2D1_ANTIALIAS_MODE_PER_PRIMITIVE };
        bool drawing{ false };

        uint32_t drawCallCount{ 0 };
        uint64_t frameNumber{ 0 };

        // Inherited via IRenderContext
        NbSize<int> measureText(const std::wstring& text, TextFormatHandle format, float maxWidth) const noexcept override;
        void drawGeometry(GeometryHandle geometry, BrushHandle brush, float strokeWidth) noexcept override;
        void fillGeometry(GeometryHandle geometry, BrushHandle brush, FillMode fillMode) noexcept override;
        void onDpiChanged(float newDpi) noexcept override;
        void onDeviceLost() noexcept override;
        RenderStats getStats() const noexcept override;
        void resetStats() noexcept override;
    };
} // namespace Renderer

#endif // NBUI_SRC_NEWRENDERER_DIRECT2DRENDERCONTEXT_HPP
