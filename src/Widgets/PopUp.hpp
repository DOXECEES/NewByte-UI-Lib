#pragma once
#include "Core.hpp"
#include "Widgets/IWidget.hpp"
#include <d2d1.h>
#include <memory>
#include <string>
#include <vector>

namespace Widgets
{
    
    struct PopupMenuItem
    {
        std::wstring text;
        NbRect<int> rect; // абсолютные координаты
    };

    struct PopupMenu
    {
        IWidget* parentWidget;
        NbRect<int> rect; // прямоугольник всего меню
        std::vector<PopupMenuItem> items;
        bool visible = true;
    };

    class PopupMenuManager
    {
    public:
        PopupMenuManager(ID2D1RenderTarget* renderTarget) : rt(renderTarget)
        {
            if (rt)
            {
                rt->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &defaultBrush);
            }
        }

        ~PopupMenuManager()
        {
            if (defaultBrush)
            {
                defaultBrush->Release();
            }
        }

        void showMenu(
            IWidget* widget,
            const std::vector<std::wstring>& itemTexts
        )
        {
            PopupMenu menu;
            menu.parentWidget = widget;

            NbRect<int> wRect = widget->getRect(); // координаты виджета в окне
            const int itemHeight = 20;
            menu.rect = {wRect.x, wRect.y + wRect.height, 200, int(itemTexts.size() * itemHeight)};

            for (size_t i = 0; i < itemTexts.size(); i++)
            {
                menu.items.push_back(
                    {itemTexts[i],
                     {menu.rect.x, menu.rect.y + int(i * itemHeight), menu.rect.width, itemHeight}}
                );
            }

            popups.push_back(std::move(menu));
        }

        void hideMenu(IWidget* widget)
        {
            for (auto it = popups.begin(); it != popups.end();)
            {
                if (it->parentWidget == widget)
                {
                    it = popups.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

        void render()
        {
            if (!rt)
            {
                return;
            }

            for (auto& menu : popups)
            {
                if (!menu.visible)
                {
                    continue;
                }

                D2D1_RECT_F rectF = D2D1::RectF(
                    float(menu.rect.x), float(menu.rect.y), float(menu.rect.x + menu.rect.width),
                    float(menu.rect.y + menu.rect.height)
                );

                // фоновый прямоугольник меню
                rt->FillRectangle(rectF, defaultBrush);

                // рендер текста
                for (auto& item : menu.items)
                {
                    // здесь можно использовать свой метод drawText из Direct2dWidgetRenderer
                    // Пример с ID2D1RenderTarget:
                     //rt->DrawTextW(item.text.c_str(), int(item.text.size()), textFormat,
                     //              D2D1::RectF(float(item.rect.x), float(item.rect.y),
                     //                          float(item.rect.x + item.rect.width),
                     //                          float(item.rect.y + item.rect.height)),
                     //              textBrush);
                }
            }
        }

        IWidget* hittest(
            int x,
            int y
        )
        {
            for (auto it = popups.rbegin(); it != popups.rend();
                 ++it) // с конца, чтобы Z-уровень соблюсти
            {
                PopupMenu& menu = *it;
                if (!menu.visible)
                {
                    continue;
                }

                if (menu.rect.isInside({x, y}))
                {
                    for (auto& item : menu.items)
                    {
                        if (item.rect.isInside({x, y}))
                        {
                            return menu.parentWidget; // или можно возвращать сам элемент меню
                        }
                    }
                }
            }
            return nullptr;
        }

        bool isPointInsideAny(const NbPoint<int>& p) const
        {
            for (const auto& menu : popups)
            {
                if (menu.visible && menu.rect.isInside(p))
                {
                    return true;
                }
            }
            return false;
        }

        bool handleClick(const NbPoint<int>& p)
        {
            for (auto it = popups.rbegin(); it != popups.rend(); ++it)
            {
                auto& menu = *it;
                if (!menu.visible)
                {
                    continue;
                }

                if (menu.rect.isInside(p))
                {
                    for (auto& item : menu.items)
                    {
                        if (item.rect.isInside(p))
                        {
                            // TODO: вызвать callback
                            // item.callback();

                            menu.visible = false;
                            return true;
                        }
                    }

                    // клик внутри popup, но не по item
                    return true;
                }
            }

            return false;
        }

        void hideAll()
        {
            popups.clear();
        }

    private:
        ID2D1RenderTarget* rt = nullptr;
        ID2D1SolidColorBrush* defaultBrush = nullptr;
        std::vector<PopupMenu> popups;
    };

} // namespace Renderer