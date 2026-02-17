#include "Direct2DRenderContext.hpp"
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <stdexcept>
#include <unordered_map>
#include "Direct2dFactory.hpp"

using namespace Microsoft::WRL;

namespace Renderer
{
    Direct2DRenderContext::Direct2DRenderContext(HWND hwnd_, IResourceManager* resourceManager_)
        : hwnd(hwnd_), resourceManager(resourceManager_)
    {
    }

    Direct2DRenderContext::~Direct2DRenderContext()
    {
        releaseDeviceResources();
    }

    Direct2DRenderContext::Direct2DRenderContext(Direct2DRenderContext&& other) noexcept
        : hwnd(other.hwnd), resourceManager(other.resourceManager),
        d2dFactory(other.d2dFactory), dwriteFactory(other.dwriteFactory), wicFactory(other.wicFactory),
        d2dDevice(std::move(other.d2dDevice)), d2dContext(std::move(other.d2dContext)),
        swapChain(std::move(other.swapChain)), targetBitmap(std::move(other.targetBitmap)),
        transformStack(std::move(other.transformStack)), clipStack(std::move(other.clipStack)),
        layerStack(std::move(other.layerStack)), dpi(other.dpi), antialiasMode(other.antialiasMode),
        drawing(other.drawing), drawCallCount(other.drawCallCount), frameNumber(other.frameNumber),
        d3dDevice(std::move(other.d3dDevice)), dxgiDevice(std::move(other.dxgiDevice))
    {
        other.drawing = false;
        other.hwnd = nullptr;
        other.resourceManager = nullptr;
        other.d2dFactory = nullptr;
        other.dwriteFactory = nullptr;
        other.wicFactory = nullptr;
    }

    Direct2DRenderContext& Direct2DRenderContext::operator=(Direct2DRenderContext&& other) noexcept
    {
        if (this != &other)
        {
            releaseDeviceResources();

            hwnd = other.hwnd;
            resourceManager = other.resourceManager;
            d2dFactory = other.d2dFactory;
            dwriteFactory = other.dwriteFactory;
            wicFactory = other.wicFactory;
            d2dDevice = std::move(other.d2dDevice);
            d2dContext = std::move(other.d2dContext);
            swapChain = std::move(other.swapChain);
            targetBitmap = std::move(other.targetBitmap);
            d3dDevice = std::move(other.d3dDevice);
            dxgiDevice = std::move(other.dxgiDevice);

            transformStack = std::move(other.transformStack);
            clipStack = std::move(other.clipStack);
            layerStack = std::move(other.layerStack);

            dpi = other.dpi;
            antialiasMode = other.antialiasMode;
            drawing = other.drawing;
            drawCallCount = other.drawCallCount;
            frameNumber = other.frameNumber;

            other.drawing = false;
            other.hwnd = nullptr;
            other.resourceManager = nullptr;
            other.d2dFactory = nullptr;
            other.dwriteFactory = nullptr;
            other.wicFactory = nullptr;
        }
        return *this;
    }

    bool Direct2DRenderContext::initialize() noexcept
    {
        if (!createDeviceResources())
            return false;

        // Создаём swap chain и target bitmap через resize
        RECT rect;
        GetClientRect(hwnd, &rect);
        return resize(rect.right - rect.left, rect.bottom - rect.top);
    }

    bool Direct2DRenderContext::resize(uint32_t width, uint32_t height) noexcept
    {
        if (!swapChain)
            return false;

        // Приостанавливаем рендеринг
        d2dContext->SetTarget(nullptr);
        targetBitmap.Reset();

        // Убедимся, что размеры не равны нулю
        if (width == 0 || height == 0)
            return true;

        HRESULT hr = swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
        if (FAILED(hr))
        {
            // Попробуем обработать потерю устройства
            onDeviceLost();
            return false;
        }

        if (!createTargetBitmap())
        {
            onDeviceLost();
            return false;
        }

        d2dContext->SetTarget(targetBitmap.Get());
        d2dContext->SetAntialiasMode(antialiasMode);
        d2dContext->SetDpi(dpi, dpi);

        return true;
    }

    void Direct2DRenderContext::beginDraw() noexcept
    {
        if (!d2dContext || !targetBitmap)
            return;

        d2dContext->SetTarget(targetBitmap.Get());
        d2dContext->BeginDraw();
        drawing = true;

    }

    bool Direct2DRenderContext::endDraw() noexcept
    {
        if (!d2dContext || !drawing)
            return false;

        HRESULT hr = d2dContext->EndDraw();
        drawing = false;

        if (hr == D2DERR_RECREATE_TARGET)
        {
            onDeviceLost();
            return false;
        }

        if (FAILED(hr))
            return false;

        if (swapChain)
        {
            hr = swapChain->Present(1, 0);
            if (FAILED(hr) && hr != DXGI_ERROR_WAS_STILL_DRAWING)
            {
                // Обработка потери устройства или изменений размера
                if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
                {
                    onDeviceLost();
                    return false;
                }
            }
        }

        ++frameNumber;
        Debug::debug(drawCallCount);

        drawCallCount = 0;
        return true;
    }

    void Direct2DRenderContext::clear(const NbColor& color) noexcept
    {
        if (!d2dContext)
            return;

        D2D1_COLOR_F d2dColor = Direct2DUtils::toD2DColor(color);
        d2dContext->Clear(&d2dColor);
    }

    void Direct2DRenderContext::flush() noexcept
    {
        if (d2dContext)
            d2dContext->Flush();
    }

    void Direct2DRenderContext::pushClip(const NbRect<int>& clipRect) noexcept
    {
        if (!d2dContext)
            return;

        D2D1_RECT_F rect = Direct2DUtils::toD2DRect(clipRect);
        clipStack.push(rect);
        d2dContext->PushAxisAlignedClip(rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    }

    void Direct2DRenderContext::popClip() noexcept
    {
        if (!d2dContext || clipStack.empty())
            return;

        clipStack.pop();
        d2dContext->PopAxisAlignedClip();
    }

    void Direct2DRenderContext::pushLayer(const NbRect<int>& bounds, float opacity) noexcept
    {
        if (!d2dContext)
            return;

        D2D1_LAYER_PARAMETERS1 params = {};
        params.contentBounds = Direct2DUtils::toD2DRect(bounds);
        params.opacity = opacity;
        params.maskAntialiasMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
        params.layerOptions = D2D1_LAYER_OPTIONS1_NONE;

        ComPtr<ID2D1Layer> layer;
        if (SUCCEEDED(d2dContext->CreateLayer(nullptr, &layer)))
        {
            layerStack.push(layer);
            d2dContext->PushLayer(&params, layer.Get());
        }
    }

    void Direct2DRenderContext::popLayer() noexcept
    {
        if (!d2dContext || layerStack.empty())
            return;

        layerStack.pop();
        d2dContext->PopLayer();
    }

    void Direct2DRenderContext::setAntialiasMode(AntialiasMode mode) noexcept
    {
        antialiasMode = static_cast<D2D1_ANTIALIAS_MODE>(mode);
        if (d2dContext)
            d2dContext->SetAntialiasMode(antialiasMode);
    }

    AntialiasMode Direct2DRenderContext::getAntialiasMode() const noexcept
    {
        return static_cast<AntialiasMode>(antialiasMode);
    }

    float Direct2DRenderContext::getDpi() const noexcept
    {
        return dpi;
    }

    NbSize<int> Direct2DRenderContext::getSize() const noexcept
    {
        if (!targetBitmap)
            return NbSize<int>{0, 0};

        D2D1_SIZE_F size = targetBitmap->GetSize();
        return NbSize<int>{
            static_cast<int>(size.width* dpi / 96.0f),
                static_cast<int>(size.height* dpi / 96.0f)
        };
    }

    bool Direct2DRenderContext::isValid() const noexcept
    {
        return d2dContext && targetBitmap && swapChain;
    }

    ID2D1DeviceContext* Direct2DRenderContext::getNativeDeviceContext() const noexcept
    {
        return d2dContext.Get();
    }

    ID2D1Factory2* Direct2DRenderContext::getNativeFactory() const noexcept
    {
        return d2dFactory.get();
    }

    // -------------------------
    // Приватные методы
    // -------------------------
    bool Direct2DRenderContext::createDeviceResources() noexcept
    {
        HRESULT hr = S_OK;

        // --- Создаём D3D11 device ---
        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0
        };

        ComPtr<ID3D11DeviceContext> context;
        D3D_FEATURE_LEVEL featureLevel;

        hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            creationFlags,
            featureLevels,
            ARRAYSIZE(featureLevels),
            D3D11_SDK_VERSION,
            &d3dDevice,
            &featureLevel,
            &context
        );

        if (FAILED(hr))
            return false;

        // --- Получаем DXGI device ---
        hr = d3dDevice.As(&dxgiDevice);
        if (FAILED(hr))
            return false;

        auto& factory = Direct2DFactory::getInstance();
        d2dFactory = factory.getD2DFactory();

        // --- Создаём D2D device и context ---
        ComPtr<ID2D1Device> device;
        hr = d2dFactory->CreateDevice(dxgiDevice.Get(), &device);
        if (FAILED(hr)) return false;

        hr = device.As(&d2dDevice);
        if (FAILED(hr)) return false;

        hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext);
        if (FAILED(hr)) return false;

        // --- Создаём swap chain ---
        ComPtr<IDXGIAdapter> dxgiAdapter;
        hr = dxgiDevice->GetAdapter(&dxgiAdapter);
        if (FAILED(hr)) return false;

        ComPtr<IDXGIFactory2> dxgiFactory;
        hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
        if (FAILED(hr)) return false;

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = max(1, clientRect.right - clientRect.left);
        swapChainDesc.Height = max(1, clientRect.bottom - clientRect.top);
        swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swapChainDesc.Stereo = FALSE;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

        hr = dxgiFactory->CreateSwapChainForHwnd(
            d3dDevice.Get(),
            hwnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain
        );
        if (FAILED(hr))
            return false;

        // --- Устанавливаем DPI ---
        dpi = static_cast<float>(GetDpiForWindow(hwnd));
        if (dpi == 0.0f) // На случай ошибки
            dpi = 96.0f;

        d2dContext->SetDpi(dpi, dpi);
        d2dContext->SetAntialiasMode(antialiasMode);

        return createTargetBitmap();
    }

    bool Direct2DRenderContext::createTargetBitmap() noexcept
    {
        if (!swapChain || !d2dContext)
            return false;

        d2dContext->SetTarget(nullptr);
        targetBitmap.Reset();

        ComPtr<IDXGISurface> backBuffer;
        HRESULT hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
        if (FAILED(hr) || !backBuffer)
            return false;

        D2D1_BITMAP_PROPERTIES1 bitmapProps = {};
        bitmapProps.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        bitmapProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
        bitmapProps.dpiX = dpi;
        bitmapProps.dpiY = dpi;
        bitmapProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

        hr = d2dContext->CreateBitmapFromDxgiSurface(backBuffer.Get(), &bitmapProps, &targetBitmap);
        if (FAILED(hr))
            return false;

        d2dContext->SetTarget(targetBitmap.Get());
        return true;
    }

    void Direct2DRenderContext::releaseDeviceResources() noexcept
    {
        d2dContext.Reset();
        targetBitmap.Reset();
        swapChain.Reset();
        d2dDevice.Reset();
        dxgiDevice.Reset();
        d3dDevice.Reset();
    }

    void Direct2DRenderContext::onDeviceLost() noexcept
    {
        releaseDeviceResources();

        // Попробуем пересоздать устройство
        if (!createDeviceResources())
        {
            // Если не удалось, ждем следующего изменения размера
            return;
        }

        RECT rect;
        GetClientRect(hwnd, &rect);
        resize(rect.right - rect.left, rect.bottom - rect.top);

        // Уведомляем менеджер ресурсов о потере устройства
        if (resourceManager)
            resourceManager->onDeviceLost();
    }

    void Direct2DRenderContext::onDpiChanged(float newDpi) noexcept
    {
        if (newDpi != dpi)
        {
            dpi = newDpi;
            if (d2dContext)
                d2dContext->SetDpi(dpi, dpi);

            // Пересоздаем target bitmap с новым DPI
            if (swapChain && d2dContext)
                createTargetBitmap();
        }
    }

    void Direct2DRenderContext::drawGeometry(GeometryHandle geometry, BrushHandle brush, float strokeWidth) noexcept
    {
        if (!d2dContext || !drawing)
            return;

        ID2D1Brush* nativeBrush = getNativeBrush(brush);
        ID2D1Geometry* nativeGeometry = nullptr;
            //static_cast<ID2D1Geometry*>(resourceManager->getNativeGeometry(geometry)) : nullptr;

        if (!nativeBrush || !nativeGeometry)
            return;

        d2dContext->DrawGeometry(nativeGeometry, nativeBrush, strokeWidth);
        ++drawCallCount;
    }

    void Direct2DRenderContext::fillGeometry(GeometryHandle geometry, BrushHandle brush, FillMode fillMode) noexcept
    {
        if (!d2dContext || !drawing)
            return;

        ID2D1Brush* nativeBrush = getNativeBrush(brush);
        ID2D1Geometry* nativeGeometry = nullptr;
         //   static_cast<ID2D1Geometry*>(resourceManager->getNativeGeometry(geometry)) : nullptr;

        if (!nativeBrush || !nativeGeometry)
            return;

        // Устанавливаем fill mode если поддерживается
        ComPtr<ID2D1DeviceContext> dc;
        if (SUCCEEDED(d2dContext.As(&dc)))
        {
            D2D1_FILL_MODE d2dFillMode = (fillMode == FillMode::Winding) ?
                D2D1_FILL_MODE_WINDING : D2D1_FILL_MODE_ALTERNATE;
            // Для ID2D1Geometry нужно установить fill mode в самой геометрии
        }

        d2dContext->FillGeometry(nativeGeometry, nativeBrush);
        ++drawCallCount;
    }

    // -----------------------------
    // Прямоугольники
    // -----------------------------
    void Direct2DRenderContext::drawRectangle(const NbRect<int>& rect, BrushHandle brush) noexcept
    {
        if (!d2dContext || !drawing)
            return;

        ID2D1Brush* nativeBrush = getNativeBrush(brush);
        if (!nativeBrush)
            return;

        D2D1_RECT_F r = Direct2DUtils::toD2DRect(rect);
        d2dContext->DrawRectangle(r, nativeBrush);
        ++drawCallCount;
    }

    void Direct2DRenderContext::fillRectangle(const NbRect<int>& rect, BrushHandle brush) noexcept
    {
        if (!d2dContext || !drawing)
            return;

        ID2D1Brush* nativeBrush = getNativeBrush(brush);
        if (!nativeBrush)
            return;

        D2D1_RECT_F r = Direct2DUtils::toD2DRect(rect);
        d2dContext->FillRectangle(r, nativeBrush);
        ++drawCallCount;
    }

    // -----------------------------
    // Скругленные прямоугольники
    // -----------------------------
    void Direct2DRenderContext::drawRoundedRectangle(const NbRect<int>& rect, float radius, BrushHandle brush) noexcept
    {
        if (!d2dContext || !drawing)
            return;

        ID2D1Brush* nativeBrush = getNativeBrush(brush);
        if (!nativeBrush)
            return;

        D2D1_ROUNDED_RECT rr = {};
        rr.rect = Direct2DUtils::toD2DRect(rect);
        rr.radiusX = radius;
        rr.radiusY = radius;
        d2dContext->DrawRoundedRectangle(rr, nativeBrush);
        ++drawCallCount;
    }

    void Direct2DRenderContext::fillRoundedRectangle(const NbRect<int>& rect, float radius, BrushHandle brush) noexcept
    {
        if (!d2dContext || !drawing)
            return;

        ID2D1Brush* nativeBrush = getNativeBrush(brush);
        if (!nativeBrush)
            return;

        D2D1_ROUNDED_RECT rr = {};
        rr.rect = Direct2DUtils::toD2DRect(rect);
        rr.radiusX = radius;
        rr.radiusY = radius;
        d2dContext->FillRoundedRectangle(rr, nativeBrush);
        ++drawCallCount;
    }

    // -----------------------------
    // Эллипсы
    // -----------------------------
    void Direct2DRenderContext::drawEllipse(const NbPoint<int>& center, float radiusX, float radiusY, BrushHandle brush) noexcept
    {
        if (!d2dContext || !drawing)
            return;

        ID2D1Brush* nativeBrush = getNativeBrush(brush);
        if (!nativeBrush)
            return;

        D2D1_ELLIPSE ellipse = {};
        ellipse.point = Direct2DUtils::toD2DPoint(NbPoint<float>{static_cast<float>(center.x), static_cast<float>(center.y)});
        ellipse.radiusX = radiusX;
        ellipse.radiusY = radiusY;
        d2dContext->DrawEllipse(ellipse, nativeBrush);
        ++drawCallCount;
    }

    void Direct2DRenderContext::fillEllipse(const NbPoint<int>& center, float radiusX, float radiusY, BrushHandle brush) noexcept
    {
        if (!d2dContext || !drawing)
            return;

        ID2D1Brush* nativeBrush = getNativeBrush(brush);
        if (!nativeBrush)
            return;

        D2D1_ELLIPSE ellipse = {};
        ellipse.point = Direct2DUtils::toD2DPoint(NbPoint<float>{static_cast<float>(center.x), static_cast<float>(center.y)});
        ellipse.radiusX = radiusX;
        ellipse.radiusY = radiusY;
        d2dContext->FillEllipse(ellipse, nativeBrush);
        ++drawCallCount;
    }

    // -----------------------------
    // Линии
    // -----------------------------
    void Direct2DRenderContext::drawLine(const NbPoint<int>& start, const NbPoint<int>& end, BrushHandle brush, float strokeWidth) noexcept
    {
        if (!d2dContext || !drawing)
            return;

        ID2D1Brush* nativeBrush = getNativeBrush(brush);
        if (!nativeBrush)
            return;

        D2D1_POINT_2F p1 = Direct2DUtils::toD2DPoint(NbPoint<float>{static_cast<float>(start.x), static_cast<float>(start.y)});
        D2D1_POINT_2F p2 = Direct2DUtils::toD2DPoint(NbPoint<float>{static_cast<float>(end.x), static_cast<float>(end.y)});
        d2dContext->DrawLine(p1, p2, nativeBrush, strokeWidth);
        ++drawCallCount;
    }

    // -----------------------------
    // Текст
    // -----------------------------
    void Direct2DRenderContext::drawText(const std::wstring& text, const NbRect<int>& layoutRect, TextFormatHandle format, BrushHandle brush) noexcept
    {
        if (!d2dContext || !drawing)
            return;

        ID2D1Brush* nativeBrush = getNativeBrush(brush);
        IDWriteTextFormat* nativeFormat = getNativeTextFormat(format);

        if (!nativeBrush || !nativeFormat)
            return;

        D2D1_RECT_F r = Direct2DUtils::toD2DRect(layoutRect);
        d2dContext->DrawTextW(text.c_str(), static_cast<UINT32>(text.size()), nativeFormat, &r, nativeBrush);
        ++drawCallCount;
    }

    NbSize<int> Direct2DRenderContext::measureText(const std::wstring& text, TextFormatHandle format, float maxWidth) const noexcept
    {
        if (!resourceManager)
            return NbSize<int>{0, 0};

        IDWriteTextFormat* nativeFormat = getNativeTextFormat(format);
        if (!nativeFormat)
            return NbSize<int>{0, 0};

        // Получаем фабрику DWrite
        auto& factory = Direct2DFactory::getInstance();
        IDWriteFactory* dwriteFactory = factory.getDWriteFactory();
        if (!dwriteFactory)
            return NbSize<int>{0, 0};

        ComPtr<IDWriteTextLayout> textLayout;
        HRESULT hr = dwriteFactory->CreateTextLayout(
            text.c_str(),
            static_cast<UINT32>(text.size()),
            nativeFormat,
            maxWidth > 0 ? maxWidth : FLT_MAX,
            FLT_MAX,
            &textLayout
        );

        if (FAILED(hr) || !textLayout)
            return NbSize<int>{0, 0};

        DWRITE_TEXT_METRICS textMetrics;
        hr = textLayout->GetMetrics(&textMetrics);
        if (FAILED(hr))
            return NbSize<int>{0, 0};

        return NbSize<int>{
            static_cast<int>(ceil(textMetrics.widthIncludingTrailingWhitespace)),
                static_cast<int>(ceil(textMetrics.height))
        };
    }

    // -----------------------------
    // Bitmap
    // -----------------------------
    void Direct2DRenderContext::drawBitmap(const NbRect<int>& destination, BitmapHandle bitmap, float opacity) noexcept
    {
        if (!d2dContext || !drawing)
            return;

        ID2D1Bitmap1* nativeBitmap = getNativeBitmap(bitmap);
        if (!nativeBitmap)
            return;

        D2D1_RECT_F dest = Direct2DUtils::toD2DRect(destination);
        d2dContext->DrawBitmap(nativeBitmap, dest, opacity);
        ++drawCallCount;
    }

    void Direct2DRenderContext::drawBitmap(const NbRect<int>& destination, const NbRect<int>& sourceRect, BitmapHandle bitmap, float opacity) noexcept
    {
        if (!d2dContext || !drawing)
            return;

        ID2D1Bitmap1* nativeBitmap = getNativeBitmap(bitmap);
        if (!nativeBitmap)
            return;

        D2D1_RECT_F dest = Direct2DUtils::toD2DRect(destination);
        D2D1_RECT_F src = Direct2DUtils::toD2DRect(sourceRect);

        d2dContext->DrawBitmap(nativeBitmap, dest, opacity, D2D1_INTERPOLATION_MODE_LINEAR, &src);
        ++drawCallCount;
    }

    // -----------------------------
    // Статистика
    // -----------------------------
    IRenderContext::RenderStats Direct2DRenderContext::getStats() const noexcept
    {
        RenderStats stats;
        stats.drawCallCount = drawCallCount;
        stats.frameNumber = frameNumber;
        //stats.isDeviceValid = isValid();

        if (d3dDevice)
        {
            //stats.deviceInfo = "Direct3D 11 / Direct2D";
        }

        return stats;
    }

    void Direct2DRenderContext::resetStats() noexcept
    {
        drawCallCount = 0;
        frameNumber = 0;
    }

    // -----------------------------
    // Вспомогательные методы
    // -----------------------------


    

    

    // Вспомогательные методы для получения нативных ресурсов
    ID2D1Brush* Direct2DRenderContext::getNativeBrush(BrushHandle handle) const noexcept
    {
        return resourceManager ? static_cast<ID2D1Brush*>(resourceManager->getNativeBrush(handle)) : nullptr;
    }

    IDWriteTextFormat* Direct2DRenderContext::getNativeTextFormat(TextFormatHandle handle) const noexcept
    {
        return resourceManager ? static_cast<IDWriteTextFormat*>(resourceManager->getNativeTextFormat(handle)) : nullptr;
    }

    ID2D1Bitmap1* Direct2DRenderContext::getNativeBitmap(BitmapHandle handle) const noexcept
    {
        return resourceManager ? static_cast<ID2D1Bitmap1*>(resourceManager->getNativeBitmap(handle)) : nullptr;
    }

    

    // Остальные методы остаются без изменений...
}