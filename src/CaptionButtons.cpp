#include "CaptionButtons.hpp"

#include "Renderer/FactorySingleton.hpp"

CaptionButtons::CaptionButtons(HWND parent)
    : parent(parent)
{
    buttons.reserve(3);

    
}

CaptionButtons::~CaptionButtons()
{

}


void CaptionButtons::addButton(CaptionButton &button)
{
    buttons.push_back(std::move(button));
    buttons.back().setLayoutIndex(buttons.size() - 1);
    calculateCaptionButtonWindowSpaceRect(buttons.back());
}

void CaptionButtons::draw(CaptionButtonRenderer &renderer)
{
    for(auto& button : buttons)
    {
        renderer.render(button);
    }
}

void CaptionButtons::onSizeChanged()
{
    for(auto& button : buttons)
    {
        calculateCaptionButtonWindowSpaceRect(button);
    }
}


void CaptionButtons::calculateCaptionButtonWindowSpaceRect(CaptionButton &button)
{

    RECT rc;
    GetClientRect(parent, &rc);

    if(button.index >= buttons.size())
        return;

    const int SPACING_BETWEEN_BUTTONS = 5;
    int offset = 0;

    offset = std::accumulate(
        buttons.begin(),
        buttons.begin() + static_cast<size_t>(button.index),
        0,
        [SPACING_BETWEEN_BUTTONS](int a, const CaptionButton& b) {
            return a + b.width + SPACING_BETWEEN_BUTTONS;
        });

    button.setWindowSpaceRect( { rc.right - button.width - offset, rc.top, rc.right - offset, rc.top + button.height } );

}

// RENDERER

CaptionButtonRenderer::CaptionButtonRenderer(ID2D1HwndRenderTarget *renderTarget)
    : renderTarget(renderTarget)
{
    IDWriteFactory* directFactory = Renderer::FactorySingleton::getDirectWriteFactory();
    directFactory->CreateTextFormat(DEFAULT_FONT, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12, L"en-us", &textFormat);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    renderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &fontBrush);
}

void CaptionButtonRenderer::changeRenderTarget(ID2D1HwndRenderTarget *renderTarget)
{
    this->renderTarget = renderTarget;
    parent = renderTarget->GetHwnd();

    SafeRelease(&fontBrush);
    renderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &fontBrush);

    for(auto& color : colorMap)
    {
        SafeRelease(&color.second);
        renderTarget->CreateSolidColorBrush(Utils::toD2D1Color(color.first), &color.second);
    }
}

void CaptionButtonRenderer::render(CaptionButton &button)
{

    if(colorMap.find(button.color) == colorMap.end())
    {
        renderTarget->CreateSolidColorBrush(Utils::toD2D1Color(button.color), &colorMap[button.color]);
    }

    if(colorMap.find(button.hoverColor) == colorMap.end())
    {
        renderTarget->CreateSolidColorBrush(Utils::toD2D1Color(button.hoverColor), &colorMap[button.hoverColor]);
    }


    RECT r = button.getWindowSpaceRect(); 
    D2D1_RECT_F buttonRect = D2D1::RectF((FLOAT)r.left, (FLOAT)r.top, (FLOAT)r.right, (FLOAT)r.bottom);
   
    if(button.isHovered)
    {
        renderTarget->FillRectangle(buttonRect, colorMap[button.hoverColor]);
    }
    else
    {
        renderTarget->FillRectangle(buttonRect, colorMap[button.color]);
    }
    renderTarget->DrawText(button.text.c_str(), static_cast<UINT32>(button.text.length()), textFormat, buttonRect, fontBrush);
    
}
