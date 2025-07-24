// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#include "CaptionButtons.hpp"

#include "Renderer/FactorySingleton.hpp"

CaptionButtonsContainer::CaptionButtonsContainer()
    :paintArea(0, 0, 0, 0)
{
    buttons.reserve(3);

    
}

CaptionButtonsContainer::~CaptionButtonsContainer()
{
    
}


void CaptionButtonsContainer::addButton(CaptionButton &button)
{
    buttons.push_back(std::move(button));
    recalculateRect(buttons.back(), buttons.size() - 1);
}


void CaptionButtonsContainer::recalculateRect(CaptionButton &button, const size_t index)
{
    const int SPACING_BETWEEN_BUTTONS = 5;
    int offset = 0;

    offset = std::accumulate(
        buttons.begin(),
        buttons.begin() + static_cast<size_t>(index),
        0,
        [SPACING_BETWEEN_BUTTONS](int a, const CaptionButton& b) {
            return a + b.width + SPACING_BETWEEN_BUTTONS;
        });

    button.rect = NbRect<int>(offset, 0, button.width, button.height);
}

bool CaptionButton::hitTest(const NbRect<int>& windowRect, const NbPoint<int> &point) const noexcept
{
    NbRect<int> newRect = {windowRect.width - rect.width - rect.x, rect.y, rect.width, rect.height};
    isHovered = point.x >= newRect.x && point.x < newRect.x + newRect.width 
        && point.y >= newRect.y && point.y < newRect.y + newRect.height;
    return isHovered;
}


void CaptionButton::onClick(const NbRect<int>& windowRect, const NbPoint<int> &point) const noexcept
{
    if(hitTest(windowRect, point)) func();
}
