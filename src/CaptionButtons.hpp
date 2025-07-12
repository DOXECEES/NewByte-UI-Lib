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
    static constexpr int DEFAULT_HEIGTH = 30;
    
    CaptionButton(const std::wstring& text, const int width, const int height = 30, const NbColor& color = {105, 105, 105}, const NbColor& hoverColor = {105, 105, 105})
        : text(text)
        , width(width)
        , height(height)
        , hoverColor(hoverColor)
        , color(color)
        , func()
        , rect(0, 0, width, height)
    {

    }

    void setFunc(std::function<void()> func)
    {
        this->func = func;
    }

    bool hitTest(const NbRect<int>& windowRect, const NbPoint<int> &point) const noexcept;
    void onClick(const NbRect<int>& windowRect, const NbPoint<int> &point) const noexcept;

    std::function<void()>   func;

    NbRect<int>             rect;
    std::wstring            text;
    int                     width;
    int                     height              = DEFAULT_HEIGTH;
    NbColor                 hoverColor;
    NbColor                 color               = { 105, 105, 105 };
    mutable bool            isHovered           = false;
    
};


class CaptionButtonsContainer
{
public:

    CaptionButtonsContainer();
    ~CaptionButtonsContainer();

    void addButton(CaptionButton& button);

    const std::vector<CaptionButton> &getButtons() const { return buttons; };
    
    const NbRect<int>& getPaintArea() const { return paintArea; };
    void setPaintArea(const NbRect<int>& paintArea) { this->paintArea = paintArea; };

    inline auto begin() { return buttons.begin(); }
    inline auto end() { return buttons.end(); }
 
    inline auto begin() const { return buttons.begin(); }
    inline auto end() const { return buttons.end(); }


private:
    void recalculateRect(CaptionButton &button, const size_t index);

private:
    NbRect<int>                                         paintArea;    
    std::vector<CaptionButton>                          buttons;
};



#endif