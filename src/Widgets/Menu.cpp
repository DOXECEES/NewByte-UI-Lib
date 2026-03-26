#include "Menu.hpp"

namespace Widgets
{
    bool Menu::hitTest(const NbPoint<int>& pos)
    {
        return rect.isInside(pos);
    }

    bool Menu::hitTestClick(const NbPoint<int>& pos) noexcept
    {
        return rect.isInside(pos);
    }

    const char* Menu::getClassName() const
    {
        return CLASS_NAME;
    }

    void Menu::addItem(
        const std::string& text,
        std::function<void()> callback
    )
    {
        items.push_back({text, {}, false, callback});
    }

    void Menu::showMenu(
        int x,
        int y
    )
    {
        isVisible = true;

        NbSize<int> maxSize = {1000, 800}; 
        const NbSize<int>& size = measure(maxSize);

        int finalX = x;
        int finalY = y;

        if (finalX + size.width > 1000)
        {
            finalX -= size.width;
        }

        if (finalY + size.height > 800)
        {
            finalY -= size.height;
        }

        layout({finalX, finalY, size.width, size.height});

    }

    void Menu::hideMenu()
    {
        isVisible = false;
    }
};


