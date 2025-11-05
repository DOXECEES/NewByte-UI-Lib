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
    Direct2dHandleRenderTarget(ID2D1HwndRenderTarget *renderTarget)
    {
        this->renderTarget = renderTarget;
        Microsoft::WRL::ComPtr<IDWriteFactory> directFactory = Renderer::FactorySingleton::getDirectWriteFactory(); 

        if(textFormat == nullptr)
        {

            wchar_t buf[8];
            GetUserDefaultLocaleName(buf, 8);

            HRESULT hr = directFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, buf, &textFormat);
            if (FAILED(hr))
            {
                OutputDebugString(std::to_wstring(hr).c_str());
            }

            textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        }


    }

    ~Direct2dHandleRenderTarget()
    {
        for (auto &color : colorMap)
        {
            //SafeRelease(&color.second);
        }
        SafeRelease(&renderTarget);
        SafeRelease(&textFormat);
    }

    void beginDraw() noexcept
    {
        renderTarget->BeginDraw();
    }

    HRESULT endDraw() noexcept
    {
        return renderTarget->EndDraw();
    }

    void clear(const NbColor &color) const noexcept
    {
        renderTarget->Clear(Direct2dUtils::toD2D1Color(color));
    }

    void resize(const NbSize<int> &size) noexcept
    {
        uint32_t dpi = GetDpiForWindow(renderTarget->GetHwnd());
        renderTarget->SetDpi(dpi, dpi);
        renderTarget->Resize(D2D1::SizeU(size.width, size.height));
    }

    void drawRectangle(const NbRect<int>& rect, const NbColor& color, const float strokeWidth = 1.0f) const noexcept
    {
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush = createSolidBrush(color);
        renderTarget->DrawRectangle(Direct2dUtils::toD2D1Rect(rect), brush.Get(), strokeWidth);
    }

    void drawRoundedRectangle(const NbRect<int>& rect, const int radius, const NbColor& color, const float strokeWidth = 1.0f) const noexcept
    {
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush = createSolidBrush(color);
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pStrokeBrush = createSolidBrush({ 0, 0, 0 });

        const D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
            Direct2dUtils::toD2D1Rect(rect),
            radius,
            radius
        );
        
        

        renderTarget->FillRoundedRectangle(&roundedRect, brush.Get());
        //renderTarget->DrawRoundedRectangle(&roundedRect, pStrokeBrush, 75.0f);

        //renderTarget->DrawRoundedRectangle(roundedRect, brush, strokeWidth, nullptr);
    }

    void drawArc(const NbPoint<int>& startPoint, const NbPoint<int>& endPoint, const int radius, const NbColor& color) const noexcept
    {
        ID2D1PathGeometry* pathGeometry = Renderer::FactorySingleton::getPathGeometry();
        ID2D1GeometrySink* sink = nullptr;
        HRESULT hr = pathGeometry->Open(&sink);
        if(hr != S_OK)
        {
            OutputDebugString(L"EndDraw failed\n");
        }

        sink->BeginFigure(Direct2dUtils::toD2D1Point(startPoint), D2D1_FIGURE_BEGIN_HOLLOW);

        D2D1_ARC_SEGMENT arc = D2D1::ArcSegment(
            Direct2dUtils::toD2D1Point(endPoint),
            D2D1::SizeF(radius, radius),
            0.0f,                            // угол поворота эллипса
            D2D1_SWEEP_DIRECTION_CLOCKWISE, // направление обхода
            D2D1_ARC_SIZE_SMALL             // малая дуга
        );

        sink->AddArc(arc);

        sink->EndFigure(D2D1_FIGURE_END_OPEN);
        sink->Close();
        sink->Release();

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush = createSolidBrush(color);

        renderTarget->DrawGeometry(pathGeometry, brush.Get(), 2.0f);
    }

    void drawLine(const NbPoint<int>& p1, const NbPoint<int>& p2, const NbColor& color) const noexcept
    {
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush = createSolidBrush(color);
        renderTarget->DrawLine(Direct2dUtils::toD2D1Point(p1), Direct2dUtils::toD2D1Point(p2), brush.Get());
    }

    void drawText(const std::wstring& text,
                    const NbRect<int>& rect,
                    const NbColor& color,
                    TextAlignment alignment = TextAlignment::CENTER,
                    ParagraphAlignment paragraphAligment = ParagraphAlignment::CENTER) const noexcept
    {
        switch (alignment)
        {
        case TextAlignment::CENTER:
            textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            break;
        case TextAlignment::LEFT:
            textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            break;
        case TextAlignment::RIGHT:
            textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
            break;
        case TextAlignment::JUSTIFY:
            textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
        default:
            break;
        }

        switch (paragraphAligment)
        {
        case ParagraphAlignment::TOP:
            textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            break;
        case ParagraphAlignment::BOTTOM:
			textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
            break;
        case ParagraphAlignment::CENTER:
			textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            break;
        default:
            break;
        }

        renderTarget->DrawText(text.c_str(), static_cast<UINT32>(text.length()), textFormat, Direct2dUtils::toD2D1Rect(rect), createSolidBrush(color).Get());
        //IDWriteTextLayout* textLayout = Direct2dWrapper::createTextLayout(text, textFormat);
        
        //IDWriteFactory *writeFactory = Renderer::FactorySingleton::getDirectWriteFactory();

        
        //IDWriteTextLayout *textLayout = nullptr;
        //writeFactory->CreateTextLayout(text.c_str(), text.length(), textFormat, rect.width, rect.height, &textLayout);

        //renderTarget->DrawTextLayout(Direct2dUtils::toD2D1Point(NbPoint<int>(rect.x, rect.y)), textLayout, createSolidBrush(color));

    }

    void drawText(IDWriteTextLayout* textLayout, const NbRect<int>& rect, const NbColor& color, TextAlignment alignment = TextAlignment::CENTER) const noexcept
    {
        setAlignment(textLayout, alignment);

        renderTarget->DrawTextLayout(Direct2dUtils::toD2D1Point(NbPoint<int>(rect.x, rect.y)), textLayout, createSolidBrush(color).Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
    }

public:
    void setAlignment(IDWriteTextFormat* format, TextAlignment alignment) const 
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

    /*void setAlignment(IDWriteTextLayout* layout, TextAlignment alignment) 
	{
		switch (alignment)
		{
		case TextAlignment::CENTER:
            layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			break;
		case TextAlignment::LEFT:
            layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			break;
		case TextAlignment::RIGHT:
            layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
			break;
		default:
			break;
		}
	}*/

    


    void fillRectangle(const NbRect<int>& rect, const NbColor& color) const noexcept
    {
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush = createSolidBrush(color);
        renderTarget->FillRectangle(Direct2dUtils::toD2D1Rect(rect), brush.Get());
    }

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> createSolidBrush(const NbColor &color) const noexcept
    {
        if (colorMap.find(color) != colorMap.end())
        {
            return colorMap.at(color);
        }

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush = nullptr;
        renderTarget->CreateSolidColorBrush(Utils::toD2D1Color(color), brush.GetAddressOf());
        colorMap[color] = brush;
        return brush;
    }

    /*Microsoft::WRL::ComPtr<ID2D1LinearGradientBrush> createLinearGradientBrush(const NbColor& color1, const NbColor& color2) const noexcept
    {
        if (linearGradientColorMap.find(color) != linearGradientColorMap.end())
        {
            return linearGradientColorMap.at(color);
        }

        Microsoft::WRL::ComPtr<ID2D1LinearGradientBrush> brush = nullptr;
        D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES props;
        props.startPoint = { 0.0f, 0.0f };
        props.endPoint = { 1.0f, 1.0f };
        
        D2D1_GRADIENT_STOP stop1[] =
        {
            {
                0.0f,
                Direct2dUtils::toD2D1Color(color1)
            },
            {
                1.0f,
                Direct2dUtils::toD2D1Color(color2)
            }
        };

        Microsoft::WRL::ComPtr<ID2D1GradientStopCollection> collection;
        renderTarget->CreateGradientStopCollection(stop1, _countof(stop1), collection.GetAddressOf());

        
      
        renderTarget->CreateLinearGradientBrush(props, collection.Get(), brush.GetAddressOf());
        
        linearGradientColorMap[color] = brush;
        return brush;
    }*/

    ID2D1HwndRenderTarget *getRawRenderTarget() const noexcept
    {
        return renderTarget;
    }

private:
    ID2D1HwndRenderTarget*                                                                  renderTarget = nullptr;
    mutable std::unordered_map<NbColor, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>>       colorMap;
    mutable std::unordered_map <NbColor, Microsoft::WRL::ComPtr<ID2D1LinearGradientBrush>>  linearGradientColorMap;
    IDWriteTextFormat*                                                                      textFormat = nullptr;
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