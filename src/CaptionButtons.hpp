#ifndef SRC_CAPTIONBUTTONS_HPP
#define SRC_CAPTIONBUTTONS_HPP

#include <Windows.h>

#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include <string>
#include <functional>
#include <numeric>

#include "Core.hpp"
#include "Utils.hpp"


struct CaptionButton
{
    friend class CaptionButtons;

    static constexpr int NOT_BOUNDED = -1;
    static constexpr int DEFAULT_HEIGTH = 30;
    
    CaptionButton(const std::wstring& text, const int width, const int height = 30, const NbColor& color = {105, 105, 105}, const NbColor& hoverColor = {105, 105, 105})
        : text(text)
        , width(width)
        , height(height)
        , hoverColor(hoverColor)
        , color(color)
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

    const RECT& getWindowSpaceRect() const { return windowSpaceRect; }

    std::function<void()>   func;

    RECT                    rect;
    std::wstring            text;
    int                     width;
    int                     height              = DEFAULT_HEIGTH;
    NbColor                 hoverColor;
    NbColor                 color               = {105, 105, 105};
    bool                    isHovered           = false;

private:

    void setLayoutIndex(const int index) { this->index = index; }
    void setWindowSpaceRect(const RECT& rect) { this->windowSpaceRect = rect; }

    RECT                    windowSpaceRect     = {0, 0, 0, 0};
    int                     index               = NOT_BOUNDED;      
};


class ICaptionButtonRenderer
{
public:
    virtual void render(CaptionButton& button) = 0;
};

class CaptionButtonRenderer : public ICaptionButtonRenderer
{
public:
    static constexpr wchar_t const* DEFAULT_FONT        = L"Segoe UI";
    static constexpr int            DEFAULT_FONT_SIZE   = 12;

    CaptionButtonRenderer(ID2D1HwndRenderTarget* renderTarget);
    ~CaptionButtonRenderer() = default;
   
    void changeRenderTarget(ID2D1HwndRenderTarget* renderTarget);
    void render(CaptionButton &button) override;

private:
    std::unordered_map<NbColor, ID2D1SolidColorBrush*>  colorMap;

    IDWriteTextFormat*                                  textFormat      = nullptr;
    ID2D1HwndRenderTarget*                              renderTarget    = nullptr;
    ID2D1SolidColorBrush*                               fontBrush       = nullptr;

    HWND                                                parent          = nullptr;

};

class CaptionButtons
{
public:

    CaptionButtons(HWND parent);
    ~CaptionButtons();

    void addButton(CaptionButton& button);
    void draw(CaptionButtonRenderer& renderer);    // should call between BeginDraw and EndDraw

    void onSizeChanged();

private:
    void calculateCaptionButtonWindowSpaceRect(CaptionButton& button);

private:
    std::vector<CaptionButton>                          buttons;
    HWND                                                parent = nullptr;
};



#endif