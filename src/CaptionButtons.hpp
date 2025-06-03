#ifndef SRC_CAPTIONBUTTONS_HPP
#define SRC_CAPTIONBUTTONS_HPP

#include <Windows.h>

#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include <string>
#include <functional>

#include "Core.hpp"
#include "Utils.hpp"

struct CaptionButton
{
    CaptionButton(const std::wstring& text, const int width, const int height = 30, const NbColor& hoverColor = {105, 105, 105})
        : text(text)
        , width(width)
        , height(height)
        , hoverColor(hoverColor)
        , func()
    {
        rect.bottom = height;
        rect.left = 0;
        rect.right = width;
        rect.top = 0;
    }

    void setFunc(std::function<void()> func)
    {
        this->func = func;
    }

    std::function<void()>   func;

    RECT                    rect;
    std::wstring            text;
    int                     width;
    int                     height      = 30;
    NbColor                 hoverColor;
    NbColor                 color = {105, 105, 105};
};




class CaptionButtons
{
public:
    CaptionButtons(ID2D1HwndRenderTarget* renderTarget);

    void changeRenderTarget(ID2D1HwndRenderTarget* renderTarget);
    void addButton(const CaptionButton& button);
    void draw();    // should call between BeginDraw and EndDraw

private:
    std::vector<CaptionButton>                          buttons;
    std::unordered_map<NbColor, ID2D1SolidColorBrush*>  colorMap;

    ID2D1HwndRenderTarget*                              renderTarget = nullptr;
};



#endif