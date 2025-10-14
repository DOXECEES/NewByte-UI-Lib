// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#include "Direct2dRenderer.hpp"
#include "Direct2dWrapper.hpp"

#include "Direct2dFont.hpp"

D2D1_RECT_F Direct2dUtils::toD2D1Rect(const NbRect<int> &rect) noexcept
{
    return D2D1::RectF(static_cast<float>(rect.x), static_cast<float>(rect.y), static_cast<float>(rect.x + rect.width), static_cast<float>(rect.y + rect.height));
}

D2D1_COLOR_F Direct2dUtils::toD2D1Color(const NbColor& color) noexcept
{
    return D2D1::ColorF(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
}

D2D1_POINT_2F Direct2dUtils::toD2D1Point(const NbPoint<int> &point) noexcept
{
    return D2D1::Point2F(static_cast<float>(point.x), static_cast<float>(point.y));
}

Direct2dHandleRenderTarget Direct2dWrapper::createRenderTarget(const NbWindowHandle &handle, const NbSize<int> &size) noexcept
{

    D2D1_SIZE_U renderTargetSize = D2D1::SizeU(size.width, size.height);
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
    D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(handle.as<HWND>(), renderTargetSize);
    
    ID2D1HwndRenderTarget *renderTarget = nullptr;
    if(factory->CreateHwndRenderTarget(props, hwndProps, &renderTarget) != S_OK)
    {
        return nullptr;
    }


    uint32_t dpi = GetDpiForWindow(handle.as<HWND>());
    renderTarget->SetDpi(dpi, dpi);

    return Direct2dHandleRenderTarget(renderTarget);
}

// Direct2dTextFormat Direct2dWrapper::createTextFormat(const std::wstring& font) noexcept
// {
//     IDWriteFactory* directFactory = Renderer::FactorySingleton::getDirectWriteFactory();
//     IDWriteTextFormat* textFormat = nullptr;
//     directFactory->CreateTextFormat(font.c_str(), nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12, L"en-us", &textFormat);
//     textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
//     textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
//     return Direct2dTextFormat(textFormat);
// }

Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> Direct2dWrapper::createSolidColorBrush(const Direct2dHandleRenderTarget &renderTarget, const NbColor &color) noexcept
{
    return renderTarget.createSolidBrush(color);
}

Microsoft::WRL::ComPtr<IDWriteTextFormat> Direct2dWrapper::createTextFormatForWidget(Widgets::IWidget* widget, const Font& font) noexcept
{
    Microsoft::WRL::ComPtr<IDWriteFactory> directFactory = Renderer::FactorySingleton::getDirectWriteFactory();
    Direct2dFont directFont(font);
    
    Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat = directFont.getTextFormat();
    Renderer::Direct2dGlobalWidgetMapper::addTextFormat(widget, textFormat);
    return textFormat;
}

// IDWriteTextLayout *Direct2dWrapper::createTextLayout(const std::wstring& text, IDWriteTextFormat* textFormat)
// {
//     IDWriteFactory *writeFactory = Renderer::FactorySingleton::getDirectWriteFactory();

//     IDWriteTextLayout *textLayout = nullptr;
//     writeFactory->CreateTextLayout(text.c_str(), text.length(), textFormat, 0, 0, &textLayout);
//     return textLayout;
// }
