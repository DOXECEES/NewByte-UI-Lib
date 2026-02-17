#ifndef NBUI_SRC_RENDERER_DIRECT2DWRAPPER_HPP
#define NBUI_SRC_RENDERER_DIRECT2DWRAPPER_HPP

#include "../Core.hpp"
#include "../WindowInterface/WindowCore.hpp"
#include "FactorySingleton.hpp"
#include "../Utils.hpp"

#include "Font.hpp"
#include "Direct2dGlobalWidgetMapper.hpp"

#include <unordered_map>
#include <memory>

#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d2d1_1.h>
#include <wrl/client.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwrite.lib")

#include "Debug.hpp"

// Используем ComPtr везде
using Microsoft::WRL::ComPtr;

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
    // Конструктор принимает HWND и размер
    Direct2dHandleRenderTarget(const NbWindowHandle& handle, const NbSize<int>& size)
        : m_hwnd(handle.as<HWND>())
    {
        InitDevice(size);
        InitTextFormat();
    }

    ~Direct2dHandleRenderTarget()
    {
        // Все ComPtr автоматически освобождаются в деструкторе
        // Важно соблюдать порядок уничтожения
        m_d2dTargetBitmap.Reset();
        m_d2dContext.Reset();
        m_swapChain.Reset();
        m_d3dDevice.Reset();
        m_d2dFactory.Reset();

        // Очищаем кэш кистей
        m_colorMap.clear();

        // Освобождаем текстовый формат
        m_textFormat.Reset();
        m_layer.Reset();
    }

    // Запрещаем копирование и присваивание
    Direct2dHandleRenderTarget(const Direct2dHandleRenderTarget&) = delete;
    Direct2dHandleRenderTarget& operator=(const Direct2dHandleRenderTarget&) = delete;

    // Разрешаем перемещение
    Direct2dHandleRenderTarget(Direct2dHandleRenderTarget&& other) noexcept
        : m_hwnd(other.m_hwnd)
        , m_d2dFactory(std::move(other.m_d2dFactory))
        , m_d3dDevice(std::move(other.m_d3dDevice))
        , m_swapChain(std::move(other.m_swapChain))
        , m_d2dContext(std::move(other.m_d2dContext))
        , m_d2dTargetBitmap(std::move(other.m_d2dTargetBitmap))
        , m_textFormat(std::move(other.m_textFormat))
        , m_layer(std::move(other.m_layer))
        , m_colorMap(std::move(other.m_colorMap))
    {
        other.m_hwnd = nullptr;
    }

    Direct2dHandleRenderTarget& operator=(Direct2dHandleRenderTarget&& other) noexcept
    {
        if (this != &other)
        {
            // Освобождаем текущие ресурсы
            this->~Direct2dHandleRenderTarget();

            // Перемещаем ресурсы
            m_hwnd = other.m_hwnd;
            m_d2dFactory = std::move(other.m_d2dFactory);
            m_d3dDevice = std::move(other.m_d3dDevice);
            m_swapChain = std::move(other.m_swapChain);
            m_d2dContext = std::move(other.m_d2dContext);
            m_d2dTargetBitmap = std::move(other.m_d2dTargetBitmap);
            m_textFormat = std::move(other.m_textFormat);
            m_layer = std::move(other.m_layer);
            m_colorMap = std::move(other.m_colorMap);

            other.m_hwnd = nullptr;
        }
        return *this;
    }

private:
    void InitDevice(const NbSize<int>& size)
    {
        HRESULT hr = S_OK;

        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0
        };

        ComPtr<ID3D11DeviceContext> context;

        hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            creationFlags,
            featureLevels,
            ARRAYSIZE(featureLevels),
            D3D11_SDK_VERSION,
            &m_d3dDevice,
            nullptr,
            &context
        );

        if (FAILED(hr))
        {
            Debug::debug(hr);
            return;
        }

        ComPtr<IDXGIDevice> dxgiDevice;
        hr = m_d3dDevice.As(&dxgiDevice);
        if (FAILED(hr)) return;

        // Получаем фабрику D2D
        m_d2dFactory = Renderer::FactorySingleton::getFactory();

        ComPtr<ID2D1Device> d2dDevice;
        hr = m_d2dFactory->CreateDevice(dxgiDevice.Get(), &d2dDevice);
        if (FAILED(hr)) return;

        hr = d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &m_d2dContext
        );
        if (FAILED(hr)) return;

        // Создаем Swap Chain
        ComPtr<IDXGIAdapter> dxgiAdapter;
        hr = dxgiDevice->GetAdapter(&dxgiAdapter);
        if (FAILED(hr)) return;

        ComPtr<IDXGIFactory2> dxgiFactory;
        hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
        if (FAILED(hr)) return;

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = size.width;
        swapChainDesc.Height = size.height;
        swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swapChainDesc.Stereo = FALSE;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.Scaling = DXGI_SCALING_NONE;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

        hr = dxgiFactory->CreateSwapChainForHwnd(
            m_d3dDevice.Get(),
            m_hwnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &m_swapChain
        );

        if (FAILED(hr))
        {
            Debug::debug(hr);
            return;
        }

        CreateBitmapFromSwapChain();

        // Установка DPI
        float dpi = static_cast<float>(GetDpiForWindow(m_hwnd));
        m_d2dContext->SetDpi(dpi, dpi);
    }

    void CreateBitmapFromSwapChain()
    {
        if (!m_swapChain || !m_d2dContext) return;

        ComPtr<IDXGISurface> dxgiBackBuffer;
        HRESULT hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));
        if (FAILED(hr)) return;

        D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)
        );

        hr = m_d2dContext->CreateBitmapFromDxgiSurface(
            dxgiBackBuffer.Get(),
            &bitmapProperties,
            &m_d2dTargetBitmap
        );

        if (SUCCEEDED(hr))
        {
            m_d2dContext->SetTarget(m_d2dTargetBitmap.Get());
        }
    }

    void InitTextFormat()
    {
        ComPtr<IDWriteFactory> directFactory = Renderer::FactorySingleton::getDirectWriteFactory();

        if (!directFactory) return;

        wchar_t localeName[LOCALE_NAME_MAX_LENGTH] = { 0 };
        GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH);

        HRESULT hr = directFactory->CreateTextFormat(
            L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            14.0f,
            localeName,
            &m_textFormat
        );

        if (SUCCEEDED(hr))
        {
            m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        else
        {
            Debug::debug(hr);
        }
    }

public:
    // --- API методы ---

    void pushClip(const NbRect<int>& rect)
    {
        if (!m_d2dFactory || !m_d2dContext) return;

        // Создаем геометрию для дырки
        ComPtr<ID2D1RectangleGeometry> holeGeometry;
        D2D1_RECT_F holeRect = Direct2dUtils::toD2D1Rect(rect);
        HRESULT hr = m_d2dFactory->CreateRectangleGeometry(holeRect, &holeGeometry);
        if (FAILED(hr)) return;

        // Создаем геометрию для всего холста
        D2D1_SIZE_F size = m_d2dContext->GetSize();
        ComPtr<ID2D1RectangleGeometry> fullScreenGeometry;
        hr = m_d2dFactory->CreateRectangleGeometry(
            D2D1::RectF(0, 0, size.width, size.height),
            &fullScreenGeometry
        );
        if (FAILED(hr)) return;

        // Создаем путь для результата вычитания
        ComPtr<ID2D1PathGeometry> pathGeometry;
        hr = m_d2dFactory->CreatePathGeometry(&pathGeometry);
        if (FAILED(hr)) return;

        ComPtr<ID2D1GeometrySink> sink;
        hr = pathGeometry->Open(&sink);
        if (FAILED(hr)) return;

        // Вычитаем: Результат = ПолныйЭкран - Дырка
        hr = fullScreenGeometry->CombineWithGeometry(
            holeGeometry.Get(),
            D2D1_COMBINE_MODE_EXCLUDE,
            nullptr,
            sink.Get()
        );

        if (SUCCEEDED(hr))
        {
            sink->Close();
        }

        // Создаем или получаем слой
        if (!m_layer)
        {
            m_d2dContext->CreateLayer(nullptr, &m_layer);
        }

        // Настраиваем параметры слоя
        D2D1_LAYER_PARAMETERS1 layerParams = D2D1::LayerParameters1(
            D2D1::InfiniteRect(),
            pathGeometry.Get(),
            D2D1_ANTIALIAS_MODE_ALIASED,
            D2D1::IdentityMatrix(),
            1.0f,
            nullptr,
            D2D1_LAYER_OPTIONS1_NONE
        );

        // Применяем слой
        m_d2dContext->PushLayer(layerParams, m_layer.Get());
    }

    void popClip() noexcept
    {
        if (m_d2dContext)
        {
            m_d2dContext->PopLayer();
        }
    }

    void beginDraw() noexcept
    {
        if (m_d2dContext)
        {
            m_d2dContext->BeginDraw();
        }
    }

    HRESULT endDraw() noexcept
    {
        if (!m_d2dContext || !m_swapChain)
            return E_FAIL;

        HRESULT hr = m_d2dContext->EndDraw();
        if (FAILED(hr))
        {
            Debug::debug(hr);
            return hr;
        }

        DXGI_PRESENT_PARAMETERS params = {};
        hr = m_swapChain->Present1(1, 0, &params);

        return hr;
    }

    void clear(const NbColor& color) const noexcept
    {
        if (m_d2dContext)
        {
            m_d2dContext->Clear(Direct2dUtils::toD2D1Color(color));
        }
    }

    void resize(const NbSize<int>& size) noexcept
    {
        if (!m_d2dContext || !m_swapChain) return;

        // Сбрасываем цель отрисовки
        m_d2dContext->SetTarget(nullptr);
        m_d2dTargetBitmap.Reset();

        HRESULT hr = m_swapChain->ResizeBuffers(
            0,
            size.width,
            size.height,
            DXGI_FORMAT_UNKNOWN,
            0
        );

        if (SUCCEEDED(hr))
        {
            CreateBitmapFromSwapChain();
        }
    }

    struct LineStyle
    {
        float strokWidth = 10.0f;
        float dotLength = 1.0f;
        float gapLength = 0.0f;
    };

    void drawStyledLine(
        const NbPoint<int>& start,
        const NbPoint<int>& end,
        const NbColor& color,
        const LineStyle& style
    )
    {
        if (!m_d2dFactory || !m_d2dContext) return;

        ComPtr<ID2D1PathGeometry> geom;
        m_d2dFactory->CreatePathGeometry(&geom);

        ComPtr<ID2D1GeometrySink> sink;
        geom->Open(&sink);

        float size = style.strokWidth * style.dotLength;
        float half = size * 0.5f;
        float halfOfBasis = style.strokWidth * 0.5f;
        float spacing = size * (1.0f - style.gapLength);

        float dx = static_cast<float>(end.x - start.x);
        float dy = static_cast<float>(end.y - start.y);
        float len = sqrtf(dx * dx + dy * dy);

        if (len < 0.001f)
        {
            sink->Close();
            return;
        }

        float nx = dx / len;
        float ny = dy / len;
        float px = -ny;
        float py = nx;
        float step = size + spacing;
        int count = static_cast<int>(std::floor(len / step)) + 1;

        for (int i = 0; i < count; i++)
        {
            if (count - 1 == 0)
            {
                continue;
            }

            float t = static_cast<float>(i) / (count - 1);
            float dist = t * len;

            D2D1_POINT_2F center = {
                static_cast<float>(start.x) + nx * dist,
                static_cast<float>(start.y) + ny * dist
            };

            D2D1_POINT_2F v0 = {
                center.x + px * half + nx * half,
                center.y + py * halfOfBasis + ny * half
            };
            D2D1_POINT_2F v1 = {
                center.x - px * half + nx * half,
                center.y - py * halfOfBasis + ny * half
            };
            D2D1_POINT_2F v2 = {
                center.x - px * half - nx * half,
                center.y - py * halfOfBasis - ny * half
            };
            D2D1_POINT_2F v3 = {
                center.x + px * half - nx * half,
                center.y + py * halfOfBasis - ny * half
            };

            sink->BeginFigure(v0, D2D1_FIGURE_BEGIN_FILLED);
            sink->AddLine(v1);
            sink->AddLine(v2);
            sink->AddLine(v3);
            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        }

        sink->Close();

        ComPtr<ID2D1SolidColorBrush> brush = createSolidBrush(color);
        if (brush)
        {
            m_d2dContext->FillGeometry(geom.Get(), brush.Get());
        }
    }

    void drawGeometry(
        const ComPtr<ID2D1Geometry>& geometry,
        const NbColor& color
    )
    {
        if (!m_d2dContext || !geometry) return;

        ComPtr<ID2D1SolidColorBrush> brush = createSolidBrush(color);
        if (brush)
        {
            m_d2dContext->DrawGeometry(geometry.Get(), brush.Get());
        }
    }

    void fillGeometry(
        const ComPtr<ID2D1Geometry>& geometry,
        const NbColor& color
    )
    {
        if (!m_d2dContext || !geometry)
        {
            return;
        }

        ComPtr<ID2D1SolidColorBrush> brush = createSolidBrush(color);
        if (brush)
        {
            m_d2dContext->FillGeometry(geometry.Get(), brush.Get());
        }
    }

    void drawRectangle(
        const NbRect<int>& rect,
        const NbColor& color,
        const float strokeWidth = 1.0f
    ) const noexcept
    {
        if (!m_d2dContext) return;

        ComPtr<ID2D1SolidColorBrush> brush = createSolidBrush(color);
        if (brush)
        {
            m_d2dContext->DrawRectangle(
                Direct2dUtils::toD2D1Rect(rect),
                brush.Get(),
                strokeWidth
            );
        }
    }

    void fillRectangle(
        const NbRect<int>& rect,
        const NbColor& color
    ) const noexcept
    {
        if (!m_d2dContext) return;

        ComPtr<ID2D1SolidColorBrush> brush = createSolidBrush(color);
        if (brush)
        {
            m_d2dContext->FillRectangle(
                Direct2dUtils::toD2D1Rect(rect),
                brush.Get()
            );
        }
    }

    void drawRoundedRectangle(
        const NbRect<int>& rect,
        const int radius,
        const NbColor& color,
        const float strokeWidth = 1.0f
    ) const noexcept
    {
        if (!m_d2dContext) return;

        ComPtr<ID2D1SolidColorBrush> brush = createSolidBrush(color);
        if (!brush) return;

        const D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
            Direct2dUtils::toD2D1Rect(rect),
            static_cast<float>(radius),
            static_cast<float>(radius)
        );

        m_d2dContext->FillRoundedRectangle(&roundedRect, brush.Get());
    }

    void drawArc(
        const NbPoint<int>& startPoint,
        const NbPoint<int>& endPoint,
        const int radius,
        const NbColor& color
    ) const noexcept
    {
        if (!m_d2dFactory || !m_d2dContext) return;

        ComPtr<ID2D1PathGeometry> pathGeometry;
        HRESULT hr = m_d2dFactory->CreatePathGeometry(&pathGeometry);
        if (FAILED(hr)) return;

        ComPtr<ID2D1GeometrySink> sink;
        hr = pathGeometry->Open(&sink);
        if (FAILED(hr)) return;

        sink->BeginFigure(
            Direct2dUtils::toD2D1Point(startPoint),
            D2D1_FIGURE_BEGIN_HOLLOW
        );

        D2D1_ARC_SEGMENT arc = D2D1::ArcSegment(
            Direct2dUtils::toD2D1Point(endPoint),
            D2D1::SizeF(static_cast<float>(radius), static_cast<float>(radius)),
            0.0f,
            D2D1_SWEEP_DIRECTION_CLOCKWISE,
            D2D1_ARC_SIZE_SMALL
        );

        sink->AddArc(arc);
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
        sink->Close();

        ComPtr<ID2D1SolidColorBrush> brush = createSolidBrush(color);
        if (brush)
        {
            m_d2dContext->DrawGeometry(
                pathGeometry.Get(),
                brush.Get(),
                2.0f
            );
        }
    }

    void drawLine(
        const NbPoint<int>& p1,
        const NbPoint<int>& p2,
        const NbColor& color
    ) const noexcept
    {
        if (!m_d2dContext) return;

        ComPtr<ID2D1SolidColorBrush> brush = createSolidBrush(color);
        if (brush)
        {
            m_d2dContext->DrawLine(
                Direct2dUtils::toD2D1Point(p1),
                Direct2dUtils::toD2D1Point(p2),
                brush.Get()
            );
        }
    }

    void drawText(
        const std::wstring& text,
        const NbRect<int>& rect,
        const NbColor& color,
        TextAlignment alignment = TextAlignment::CENTER,
        ParagraphAlignment paragraphAlignment = ParagraphAlignment::CENTER
    ) const noexcept
    {
        if (!m_d2dContext || !m_textFormat) return;

        // Устанавливаем выравнивание
        switch (alignment)
        {
        case TextAlignment::CENTER:
            m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            break;
        case TextAlignment::LEFT:
            m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            break;
        case TextAlignment::RIGHT:
            m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
            break;
        case TextAlignment::JUSTIFY:
            m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
            break;
        }

        switch (paragraphAlignment)
        {
        case ParagraphAlignment::TOP:
            m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            break;
        case ParagraphAlignment::BOTTOM:
            m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
            break;
        case ParagraphAlignment::CENTER:
            m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            break;
        }

        ComPtr<ID2D1SolidColorBrush> brush = createSolidBrush(color);
        if (brush)
        {
            m_d2dContext->DrawText(
                text.c_str(),
                static_cast<UINT32>(text.length()),
                m_textFormat.Get(),
                Direct2dUtils::toD2D1Rect(rect),
                brush.Get()
            );
            


        }
    }

    void drawText(
        IDWriteTextLayout* textLayout,
        const NbRect<int>& rect,
        const NbColor& color,
        TextAlignment alignment = TextAlignment::CENTER
    ) const noexcept
    {
        if (!m_d2dContext || !textLayout) return;

        // Устанавливаем выравнивание для layout
        switch (alignment)
        {
        case TextAlignment::CENTER:
            textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            break;
        case TextAlignment::LEFT:
            textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            break;
        case TextAlignment::RIGHT:
            textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
            break;
        }

        ComPtr<ID2D1SolidColorBrush> brush = createSolidBrush(color);
        if (brush)
        {
            m_d2dContext->DrawTextLayout(
                Direct2dUtils::toD2D1Point(NbPoint<int>(rect.x, rect.y)),
                textLayout,
                brush.Get(),
                D2D1_DRAW_TEXT_OPTIONS_CLIP
            );
        }
    }

    ComPtr<ID2D1SolidColorBrush> createSolidBrush(const NbColor& color) const noexcept
    {
        if (!m_d2dContext) return nullptr;

        // Проверяем кэш
        auto it = m_colorMap.find(color);
        if (it != m_colorMap.end() && it->second)
        {
            return it->second;
        }

        // Создаем новую кисть
        ComPtr<ID2D1SolidColorBrush> brush;
        HRESULT hr = m_d2dContext->CreateSolidColorBrush(
            Direct2dUtils::toD2D1Color(color),
            &brush
        );

        if (SUCCEEDED(hr))
        {
            // Добавляем в кэш (используем const_cast, так как метод константный,
            // но кэш должен быть mutable для логической константности)
            const_cast<std::unordered_map<NbColor, ComPtr<ID2D1SolidColorBrush>>&>(m_colorMap)[color] = brush;
            return brush;
        }

        return nullptr;
    }

    // Геттер для сырого контекста
    ID2D1DeviceContext* getRawContext() const noexcept
    {
        return m_d2dContext.Get();
    }

private:
    HWND m_hwnd = nullptr;
    ComPtr<ID2D1Factory1> m_d2dFactory;
    ComPtr<ID3D11Device> m_d3dDevice;
    ComPtr<IDXGISwapChain1> m_swapChain;
    ComPtr<ID2D1DeviceContext> m_d2dContext;
    ComPtr<ID2D1Bitmap1> m_d2dTargetBitmap;
    ComPtr<IDWriteTextFormat> m_textFormat;
    ComPtr<ID2D1Layer> m_layer;

    mutable std::unordered_map<NbColor, ComPtr<ID2D1SolidColorBrush>> m_colorMap;
};

class Direct2dWrapper
{

public:
    static Direct2dHandleRenderTarget createRenderTarget(const NbWindowHandle& handle, const NbSize<int>& size) noexcept;
    //static Direct2dTextFormat createTextFormat(const std::wstring& font) noexcept
    static Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> createSolidColorBrush(const Direct2dHandleRenderTarget& renderTarget, const NbColor& color) noexcept;
    //static IDWriteTextLayout* createTextLayout(const std::wstring& text, IDWriteTextFormat* textFormat = nullptr);
    static Microsoft::WRL::ComPtr<IDWriteTextFormat> createTextFormatForWidget(Widgets::IWidget* widget, const Font& font) noexcept;

private:
    inline static Microsoft::WRL::ComPtr<ID2D1Factory> factory = Renderer::FactorySingleton::getFactory();
};

#endif////////////////////////////////////////////////////////////////