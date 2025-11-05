#ifndef NBUI_SRC_RENDERER_DIRECT2DWRAPPER_HPP
#define NBUI_SRC_RENDERER_DIRECT2DWRAPPER_HPP

#include "../Core.hpp"
#include "../WindowInterface/WindowCore.hpp"
#include "FactorySingleton.hpp"
#include "../Utils.hpp"

#include "Font.hpp"

#include "Direct2dGlobalWidgetMapper.hpp"

#include <unordered_map>

#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include <wrl.h>

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "dxgi")

#include <d3d11.h>
#include <dxgi1_2.h>
#include <d2d1_1.h>
#include <dwrite.h>
class Direct2dWrapper;


namespace Direct2dUtils
{
    D2D1_RECT_F toD2D1Rect(const NbRect<int>& rect) noexcept;
    D2D1_COLOR_F toD2D1Color(const NbColor& color) noexcept;
    D2D1_POINT_2F toD2D1Point(const NbPoint<int>& point) noexcept;

}

enum class TextAlignment
{
    CENTER,
    LEFT,
    RIGHT, 
    JUSTIFY
};

enum class ParagraphAlignment
{
    TOP,
    BOTTOM,
    CENTER,
};

class Direct2dHandleRenderTarget
{
public:
    Direct2dHandleRenderTarget(HWND hwnd)
    {
        createDeviceResources(hwnd);
        createTextFormat();
    }

    ~Direct2dHandleRenderTarget()
    {
        SafeRelease(&textFormat);
    }

    void beginDraw() noexcept
    {
        deviceContext->BeginDraw();
    }

    HRESULT endDraw() noexcept
    {
        HRESULT hr = deviceContext->EndDraw();
        swapChain->Present(1, 0); // VSync
        return hr;
    }

    void clear(const NbColor& color) const noexcept
    {
        deviceContext->Clear(D2D1::ColorF(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f));
    }

    void resize(const NbSize<int>& size) noexcept
    {
        if (!swapChain) return;

        deviceContext->SetTarget(nullptr);
        renderTargetBitmap.Reset();

        swapChain->ResizeBuffers(2, size.width, size.height, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
        createBackBufferTarget();
    }

    void drawRectangle(const NbRect<int>& rect, const NbColor& color, float strokeWidth = 1.0f) const noexcept
    {
        auto brush = createSolidBrush(color);
        deviceContext->DrawRectangle(D2D1::RectF(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height), brush.Get(), strokeWidth);
    }

    void fillRectangle(const NbRect<int>& rect, const NbColor& color) const noexcept
    {
        auto brush = createSolidBrush(color);
        deviceContext->FillRectangle(D2D1::RectF(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height), brush.Get());
    }

    // ✅ drawLine
    void drawLine(const NbPoint<int>& p1, const NbPoint<int>& p2, const NbColor& color, float strokeWidth = 1.0f) const noexcept
    {
        auto brush = createSolidBrush(color);
        deviceContext->DrawLine(Direct2dUtils::toD2D1Point(p1), Direct2dUtils::toD2D1Point(p2), brush.Get(), strokeWidth);
    }

    // ✅ drawText
    void drawText(const std::wstring& text, const NbRect<int>& rect, const NbColor& color, TextAlignment textAligment = TextAlignment::CENTER, ParagraphAlignment paragraphAligment = ParagraphAlignment::CENTER) const noexcept
    {
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush = createSolidBrush(color);

        Microsoft::WRL::ComPtr<IDWriteTextFormat> fmt;
        Renderer::FactorySingleton::getDirectWriteFactory()->CreateTextFormat(
            L"Segoe UI", nullptr,
            DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            14, L"en-us", &fmt
        );
        setAlignment(fmt.Get(), textAligment);
        fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        D2D1_RECT_F layout = D2D1::RectF(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
        deviceContext->DrawTextW(text.c_str(), (UINT32)text.length(), fmt.Get(), &layout, brush.Get());
    }

    void drawText(
        IDWriteTextLayout* textLayout,
        const NbRect<int>& rect,
        const NbColor& color,
        TextAlignment alignment = TextAlignment::CENTER
    ) const noexcept
    {
        setAlignment(textLayout, alignment);

        auto brush = createSolidBrush(color);
        deviceContext->DrawTextLayout(
            D2D1::Point2F((FLOAT)rect.x, (FLOAT)rect.y),
            textLayout,
            brush.Get(),
            D2D1_DRAW_TEXT_OPTIONS_CLIP
        );
    }


    // --- Старая версия (для IDWriteTextFormat*) ---
    void setAlignment(IDWriteTextFormat* format, TextAlignment alignment) const noexcept
    {
        switch (alignment)
        {
        case TextAlignment::CENTER:
            format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            break;
        case TextAlignment::LEFT:
            format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            break;
        case TextAlignment::RIGHT:
            format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
            break;
        default:
            break;
        }
    }

    // --- Новая версия (для IDWriteTextLayout*) ---
    void setAlignment(IDWriteTextLayout* layout, TextAlignment alignment) const noexcept
    {
        switch (alignment)
        {
        case TextAlignment::CENTER:
            layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            break;
        case TextAlignment::LEFT:
            layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            break;
        case TextAlignment::RIGHT:
            layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
            layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            break;
        default:
            break;
        }
    }



    // ✅ drawRoundedRectangle
    void drawRoundedRectangle(const NbRect<int>& rect, float radius, const NbColor& color, float strokeWidth = 1.0f) const noexcept
    {
        auto brush = createSolidBrush(color);
        D2D1_ROUNDED_RECT rounded = D2D1::RoundedRect(D2D1::RectF(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height), radius, radius);
        deviceContext->DrawRoundedRectangle(&rounded, brush.Get(), strokeWidth);
    }

    void fillRoundedRectangle(const NbRect<int>& rect, float radius, const NbColor& color) const noexcept
    {
        auto brush = createSolidBrush(color);
        D2D1_ROUNDED_RECT rounded = D2D1::RoundedRect(D2D1::RectF(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height), radius, radius);
        deviceContext->FillRoundedRectangle(&rounded, brush.Get());
    }

    // ✅ getRawRenderTarget (для старого кода)
    ID2D1RenderTarget* getRawRenderTarget() const noexcept
    {
        return deviceContext.Get();
    }

    // ✅ цветовые кисти
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> createSolidBrush(const NbColor& color) const noexcept
    {
        if (colorMap.find(color) != colorMap.end())
            return colorMap.at(color);

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        deviceContext->CreateSolidColorBrush(D2D1::ColorF(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f), brush.GetAddressOf());
        colorMap[color] = brush;
        return brush;
    }

private:
    void createDeviceResources(HWND hwnd)
    {
        Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
        Microsoft::WRL::ComPtr<ID2D1Factory1> factory1;

        // ⚙️ Исправление твоей ошибки: правильное получение ID2D1Factory1
        {
            Microsoft::WRL::ComPtr<ID2D1Factory> baseFactory = Renderer::FactorySingleton::getFactory();
            baseFactory.As(&factory1); // конвертируем безопасно
        }

        Microsoft::WRL::ComPtr<ID2D1Device> d2dDevice;

        // Создание D3D11 устройства
        Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3dContext;
        D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
        D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT, levels, _countof(levels),
            D3D11_SDK_VERSION, &d3dDevice, nullptr, &d3dContext);

        d3dDevice.As(&dxgiDevice);
        factory1->CreateDevice(dxgiDevice.Get(), &d2dDevice);
        d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &deviceContext);

        Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
        dxgiDevice->GetAdapter(&adapter);

        Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory;
        adapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory);

        DXGI_SWAP_CHAIN_DESC1 desc = {};
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferCount = 2;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        dxgiFactory->CreateSwapChainForHwnd(d3dDevice.Get(), hwnd, &desc, nullptr, nullptr, &swapChain);

        createBackBufferTarget();
    }

    void createBackBufferTarget()
    {
        Microsoft::WRL::ComPtr<IDXGISurface> dxgiBackBuffer;
        swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));

        D2D1_BITMAP_PROPERTIES1 props =
            D2D1::BitmapProperties1(
                D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

        deviceContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer.Get(), &props, &renderTargetBitmap);
        deviceContext->SetTarget(renderTargetBitmap.Get());
    }

    void createTextFormat()
    {
        Microsoft::WRL::ComPtr<IDWriteFactory> dwFactory = Renderer::FactorySingleton::getDirectWriteFactory();

        wchar_t locale[8];
        GetUserDefaultLocaleName(locale, 8);

        dwFactory->CreateTextFormat(
            L"Segoe UI", nullptr,
            DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, 14.0f, locale, &textFormat);

        textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }

private:
    Microsoft::WRL::ComPtr<ID2D1DeviceContext> deviceContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> renderTargetBitmap;

    mutable std::unordered_map<NbColor, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>> colorMap;
    IDWriteTextFormat* textFormat = nullptr;
};




// class Direct2dTextFormat
// {
// public:
//     Direct2dTextFormat(IDWriteTextFormat *textFormat) noexcept
//     {
//         this->textFormat = textFormat;
//     }

//     ~Direct2dTextFormat() noexcept
//     {
//         SafeRelease(&textFormat);
//     }

//     IDWriteTextFormat *getRawTextFormat() const noexcept
//     {
//         return textFormat;
//     }

// private:
//     IDWriteTextFormat *textFormat = nullptr;
// };

class Direct2dWrapper
{

public:
    static Direct2dHandleRenderTarget createRenderTarget(const NbWindowHandle &handle, const NbSize<int> &size) noexcept;
    //static Direct2dTextFormat createTextFormat(const std::wstring& font) noexcept
    static Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> createSolidColorBrush(const Direct2dHandleRenderTarget &renderTarget, const NbColor &color) noexcept;
    //static IDWriteTextLayout* createTextLayout(const std::wstring& text, IDWriteTextFormat* textFormat = nullptr);
    static Microsoft::WRL::ComPtr<IDWriteTextFormat> createTextFormatForWidget(Widgets::IWidget* widget, const Font& font) noexcept;

private:
    inline static Microsoft::WRL::ComPtr<ID2D1Factory> factory = Renderer::FactorySingleton::getFactory();
};

#endif