#pragma once

#include <d2d1.h>
#include <dwrite.h>
#include <functional>
#include <string>
#include <vector>
#include <windows.h>
#include <windowsx.h>
#include <wrl/client.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#include "Renderer/FactorySingleton.hpp"

namespace nbui
{

    enum class PopupStyle
    {
        ContextMenu, 
        MenuBarItem  
    };

    enum class IconType
    {
        None,
        Plus,
        Edit,
        Delete
    };
    enum class ItemType
    {
        Action,
        Separator
    };

    struct PopupItem
    {
        std::wstring          text;
        std::function<void()> callback;
        IconType              icon = IconType::None;
        ItemType              type = ItemType::Action;
    };

    class PopupMenu
    {
    public:
        void addItem(
            const std::wstring&   text,
            IconType              icon,
            std::function<void()> cb
        )
        {
            items.push_back({text, cb, icon, ItemType::Action});
        }

        void addSeparator()
        {
            items.push_back({L"", nullptr, IconType::None, ItemType::Separator});
        }

        const std::vector<PopupItem>& getItems() const
        {
            return items;
        }

    private:
        std::vector<PopupItem> items;
    };

    class PopupManager
    {
    public:
        PopupManager() : factory(Renderer::FactorySingleton::getFactory().Get())
        {
        }

        ~PopupManager()
        {
            if (popupRT)
            {
                popupRT->Release();
            }
            if (popupHwnd)
            {
                DestroyWindow(popupHwnd);
            }
        }

        void init(HWND parent)
        {
            this->parentHwnd = parent;
            createWindow();
        }

        void show(
            PopupMenu* menu,
            int        x,
            int        y,
            PopupStyle style = PopupStyle::ContextMenu
        ) const
        {
            activePopup  = menu;
            currentStyle = style; 
            hoveredIndex = -1;

            int totalHeight = 0;
            for (const auto& item : menu->getItems())
            {
                totalHeight += (item.type == ItemType::Separator) ? sepHeight : itemHeight;
            }

            width  = (currentStyle == PopupStyle::MenuBarItem) ? 200 : 300;
            height = totalHeight;

            SetWindowPos(popupHwnd, HWND_TOPMOST, x, y, width, height, SWP_SHOWWINDOW);
            createRenderTarget();
            render();

        }

        void hide() const
        {
            ShowWindow(popupHwnd, SW_HIDE);
            activePopup = nullptr;
        }

    private:
        HWND popupHwnd  = nullptr;
        HWND parentHwnd = nullptr;

        ID2D1Factory*                                     factory = nullptr;
        mutable ID2D1HwndRenderTarget*                    popupRT = nullptr;
        mutable Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat;
        mutable PopupMenu*                                activePopup = nullptr;
        mutable PopupStyle                                currentStyle = PopupStyle::ContextMenu;

        mutable Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> textBrush;
        mutable Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> hoverBrush;
        mutable Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> borderBrush;
        mutable Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> iconPlusBrush;
        mutable Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> iconEditBrush;
        mutable Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> iconDeleteBrush;
        mutable Microsoft::WRL::ComPtr<ID2D1StrokeStyle>     dashedStyle;

        mutable int width  = 220;
        mutable int height = 0;

        const int   itemHeight   = 24; 
        const int   sepHeight    = 6;  
        mutable int hoveredIndex = -1;

    private:
        static LRESULT CALLBACK WndProc(
            HWND   hwnd,
            UINT   msg,
            WPARAM wParam,
            LPARAM lParam
        )
        {
            PopupManager* self =
                reinterpret_cast<PopupManager*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

            switch (msg)
            {
            case WM_CREATE:
            {
                CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
                SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
                return 0;
            }
            case WM_MOUSEMOVE:
            {
                if (!self || !self->activePopup)
                {
                    return 0;
                }
                int mouseY = GET_Y_LPARAM(lParam);

                int currentY = 0;
                int newIndex = -1;
                for (int i = 0; i < (int)self->activePopup->getItems().size(); ++i)
                {
                    int h = (self->activePopup->getItems()[i].type == ItemType::Separator)
                                ? self->sepHeight
                                : self->itemHeight;
                    if (mouseY >= currentY && mouseY < currentY + h)
                    {
                        if (self->activePopup->getItems()[i].type != ItemType::Separator)
                        {
                            newIndex = i;
                        }
                        break;
                    }
                    currentY += h;
                }

                if (newIndex != self->hoveredIndex)
                {
                    self->hoveredIndex = newIndex;
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
                return 0;
            }
            case WM_LBUTTONDOWN:
            {
                if (self && self->activePopup && self->hoveredIndex != -1)
                {
                    auto& item = self->activePopup->getItems()[self->hoveredIndex];
                    if (item.callback)
                    {
                        item.callback();
                    }
                    self->hide();
                }
                return 0;
            }
            case WM_KILLFOCUS:
                if (self)
                {
                    self->hide();
                }
                return 0;
            case WM_PAINT:
                if (self)
                {
                    self->render();
                }
                ValidateRect(hwnd, nullptr);
                return 0;
            }
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }

        void createWindow()
        {
            WNDCLASS wc      = {};
            wc.lpfnWndProc   = WndProc;
            wc.hInstance     = GetModuleHandle(nullptr);
            wc.lpszClassName = L"NBUI_POPUP_WINDOW";
            wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
            RegisterClass(&wc);

            popupHwnd = CreateWindowEx(
                WS_EX_TOPMOST | WS_EX_TOOLWINDOW, wc.lpszClassName, L"", WS_POPUP, 0, 0, 0, 0,
                parentHwnd, nullptr, wc.hInstance, this
            );
        }

        void createRenderTarget() const
        {
            if (popupRT)
            {
                return;
            }

            factory->CreateHwndRenderTarget(
                D2D1::RenderTargetProperties(),
                D2D1::HwndRenderTargetProperties(popupHwnd, D2D1::SizeU(width, height)), &popupRT
            );

            popupRT->CreateSolidColorBrush(D2D1::ColorF(0.9f, 0.9f, 0.9f), &textBrush);
            popupRT->CreateSolidColorBrush(D2D1::ColorF(0.25f, 0.25f, 0.25f), &hoverBrush);
            popupRT->CreateSolidColorBrush(D2D1::ColorF(0.15f, 0.15f, 0.15f), &borderBrush);

            popupRT->CreateSolidColorBrush(
                D2D1::ColorF(0.53f, 0.35f, 0.85f), &iconPlusBrush
            ); 
            popupRT->CreateSolidColorBrush(
                D2D1::ColorF(0.50f, 0.38f, 0.25f), &iconEditBrush
            ); 
            popupRT->CreateSolidColorBrush(
                D2D1::ColorF(0.50f, 0.38f, 0.25f), &iconDeleteBrush
            ); 

            float dashes[] = {2.0f, 2.0f};
            factory->CreateStrokeStyle(
                D2D1::StrokeStyleProperties(
                    D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_FLAT,
                    D2D1_LINE_JOIN_MITER, 10.0f, D2D1_DASH_STYLE_CUSTOM, 0.0f
                ),
                dashes, ARRAYSIZE(dashes), &dashedStyle
            );

            Microsoft::WRL::ComPtr<IDWriteFactory> writeFactory =
                Renderer::FactorySingleton::getDirectWriteFactory();
            writeFactory->CreateTextFormat(
                L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL, 11.5f, L"ru-RU", &textFormat 
            );

            textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }

        void drawIcon(
            IconType    type,
            D2D1_RECT_F rect
        ) const
        {
            if (type == IconType::None)
            {
                return;
            }

            float iconSize   = 16.0f;
            float leftMargin = 8.0f;
            float topMargin  = (itemHeight - iconSize) / 2.0f;

            D2D1_RECT_F box = D2D1::RectF(
                rect.left + leftMargin, rect.top + topMargin, rect.left + leftMargin + iconSize,
                rect.top + topMargin + iconSize
            );

            // ОТЛИЧИЕ 1: Рисуем пунктирную рамку только для контекстного меню
            if (currentStyle == PopupStyle::ContextMenu)
            {
                popupRT->DrawRectangle(box, textBrush.Get(), 0.5f, dashedStyle.Get());
            }

            float midX = box.left + iconSize / 2.0f;
            float midY = box.top + iconSize / 2.0f;
            float s    = 3.0f;

            // Рисование иконок (без изменений)
            if (type == IconType::Plus)
            {
                popupRT->DrawLine({midX - s, midY}, {midX + s, midY}, iconPlusBrush.Get(), 1.5f);
                popupRT->DrawLine({midX, midY - s}, {midX, midY + s}, iconPlusBrush.Get(), 1.5f);
            }
            else if (type == IconType::Edit)
            {
                popupRT->DrawLine(
                    {midX - s, midY + s}, {midX + s, midY - s}, iconEditBrush.Get(), 1.2f
                );
            }
            else if (type == IconType::Delete)
            {
                popupRT->DrawLine(
                    {midX - s, midY - s}, {midX + s, midY + s}, iconDeleteBrush.Get(), 1.2f
                );
                popupRT->DrawLine(
                    {midX + s, midY - s}, {midX - s, midY + s}, iconDeleteBrush.Get(), 1.2f
                );
            }
        }

        void render() const
        {
            if (!popupRT || !activePopup)
            {
                return;
            }

            popupRT->BeginDraw();

            // ОТЛИЧИЕ 2: Разный фон
            if (currentStyle == PopupStyle::MenuBarItem)
            {
                popupRT->Clear(D2D1::ColorF(0.18f, 0.18f, 0.18f)); // Чуть светлее
            }
            else
            {
                popupRT->Clear(D2D1::ColorF(0.12f, 0.12f, 0.12f)); // Глубокий темный
            }

            float currentY = 0;
            int   index    = 0;

            for (const auto& item : activePopup->getItems())
            {
                float h = (item.type == ItemType::Separator) ? (float)sepHeight : (float)itemHeight;
                D2D1_RECT_F rect = D2D1::RectF(0, currentY, (float)width, currentY + h);

                if (item.type == ItemType::Separator)
                {
                    float midY = currentY + h / 2.0f;
                    popupRT->DrawLine(
                        {10, midY}, {(float)width - 10, midY}, borderBrush.Get(), 1.0f
                    );
                }
                else
                {
                    if (index == hoveredIndex)
                    {
                        popupRT->FillRectangle(rect, hoverBrush.Get());
                    }

                    drawIcon(item.icon, rect);

                    D2D1_RECT_F textRect = rect;
                    // ОТЛИЧИЕ 3: Отступы текста
                    float textOffset = (item.icon != IconType::None) ? 30.0f : 12.0f;
                    textRect.left += textOffset;

                    popupRT->DrawTextW(
                        item.text.c_str(), (UINT32)item.text.size(), textFormat.Get(), textRect,
                        textBrush.Get()
                    );
                }
                currentY += h;
                index++;
            }

            popupRT->DrawRectangle(
                D2D1::RectF(0, 0, (float)width, currentY), borderBrush.Get(), 1.0f
            );
            popupRT->EndDraw();
        }
    };
} // namespace nbui