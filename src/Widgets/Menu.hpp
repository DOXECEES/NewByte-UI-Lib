#ifndef NBUI_SRC_WIDGETS_MENU_HPP
#define NBUI_SRC_WIDGETS_MENU_HPP

#include "IWidget.hpp"

#include <vector>

namespace Widgets
{
	class Menu : public IWidget
    {
    public:

        Menu() noexcept : IWidget({}, 1)
        {};
        ~Menu() noexcept {};

        NB_NON_COPYMOVABLE(Menu);


        DECLARE_WIDGET_CLASS_NAME(Menu);

        struct Item
        {
            std::string text = "";
            NbRect<int> rect = {};
            bool hovered = false;
            std::function<void()> callback;
        };

        bool hitTest(const NbPoint<int>& pos) override;
        bool hitTestClick(const NbPoint<int>& pos) noexcept override;

        const char* getClassName() const override;

        void addItem(
            const std::string& text,
            std::function<void()> callback
        );
        void showMenu(
            int x,
            int y
        );
        void hideMenu();

        const std::vector<Item>& getItems() const noexcept
        {
            return items;
        }

        bool isMenuVisible() const noexcept
        {
            return isVisible;
        }

        const NbSize<int>& measure(const NbSize<int>& max) noexcept
        {
            int width = 0;
            int height = 0;

            for (const auto& item : items)
            {
                NbSize<int> textSize = {100,30}; 

                width = std::max(width, textSize.width);
                height += textSize.height + 1;
            }

            width += 1 * 2;

            measuredSize = {std::min(width, max.width), std::min(height, max.height)};

            return measuredSize;
        }

        void layout(const NbRect<int>& rect) noexcept
        {
            this->rect = rect;

            int yOffset = rect.y;

            for (auto& item : items)
            {
                int itemHeight = 30;

                item.rect = {rect.x, yOffset, rect.width, itemHeight};

                yOffset += itemHeight;
            }
        }

    private:
        

        std::vector<Item> items;
        bool isVisible = false;

    };

};


#endif