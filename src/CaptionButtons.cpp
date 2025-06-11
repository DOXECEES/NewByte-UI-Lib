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



