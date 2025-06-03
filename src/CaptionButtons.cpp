#include "CaptionButtons.hpp"



CaptionButtons::CaptionButtons(ID2D1HwndRenderTarget *renderTarget)
    :renderTarget(renderTarget)
{
    buttons.reserve(3);
}

void CaptionButtons::changeRenderTarget(ID2D1HwndRenderTarget *renderTarget)
{
    this->renderTarget = renderTarget;
    for(auto& color : colorMap)
    {
        SafeRelease(&color.second);
        renderTarget->CreateSolidColorBrush(Utils::toD2D1Color(color.first), &color.second);
    }
}

void CaptionButtons::addButton(const CaptionButton &button)
{
    buttons.push_back(button);

    if(colorMap.find(button.color) == colorMap.end())
    {
        renderTarget->CreateSolidColorBrush(Utils::toD2D1Color(button.color), &colorMap[button.color]);
    }

    if(colorMap.find(button.hoverColor) == colorMap.end())
    {
        renderTarget->CreateSolidColorBrush(Utils::toD2D1Color(button.hoverColor), &colorMap[button.hoverColor]);
    }

}

void CaptionButtons::draw()
{
    HWND parent = renderTarget->GetHwnd();

    RECT rc;
    GetClientRect(parent, &rc);
   
    int widthOffset = 0;
    const int SPACING_BETWEEN_BUTTONS = 5;


    for (const auto& button : buttons)
    {
        D2D1_RECT_F buttonRect = D2D1::RectF(static_cast<float>(rc.right - button.width - widthOffset), static_cast<float>(rc.top),
            static_cast<float>(rc.right - widthOffset), static_cast<float>(rc.top + button.height));
        
        renderTarget->FillRectangle(buttonRect, colorMap[button.color]);

        widthOffset += button.width;
        widthOffset += SPACING_BETWEEN_BUTTONS;
    }
}
