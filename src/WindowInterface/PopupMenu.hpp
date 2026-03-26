#pragma once

#include <d2d1.h>
#include <functional>
#include <string>
#include <vector>
#include <windows.h>
#include <windowsx.h>

#pragma comment(lib, "d2d1.lib")

#include "Renderer/FactorySingleton.hpp"

namespace nbui
{

    struct PopupItem
    {
        std::wstring text;
        std::function<void()> callback;
    };

    class PopupMenu
    {
    public:
        void addItem(
            const std::wstring& text,
            std::function<void()> cb
        )
        {
            items.push_back({text, cb});
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
            int x,
            int y
        ) const
        {
            activePopup = menu;

            int width = 180;
            int height = int(menu->getItems().size() * itemHeight);

            SetWindowPos(popupHwnd, HWND_TOPMOST, x, y, width, height, SWP_SHOWWINDOW);

            createRenderTarget();
            render();
        }

        void hide()
        {
            ShowWindow(popupHwnd, SW_HIDE);
            activePopup = nullptr;
        }

    private:
        HWND popupHwnd = nullptr;
        HWND parentHwnd = nullptr;

        ID2D1Factory* factory = nullptr;
        mutable ID2D1HwndRenderTarget* popupRT = nullptr;
        mutable Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat;
        mutable PopupMenu* activePopup = nullptr;
        mutable Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        mutable Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> hoverBrush;
        mutable Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> borderBrush;

        const int itemHeight = 20;
        mutable int hoveredIndex = -1;
    private:
        static LRESULT CALLBACK WndProc(
            HWND hwnd,
            UINT msg,
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
                self = (PopupManager*)cs->lpCreateParams;
                SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)self);
                return 0;
            }
            case WM_MOUSEMOVE:
            {
                int mouseY = GET_Y_LPARAM(lParam);

                int newIndex = mouseY / self->itemHeight;

                if (mouseY < 0 ||
                    mouseY > (int)(self->activePopup->getItems().size() * self->itemHeight))
                {
                    newIndex = -1;
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
                if (!self || !self->activePopup)
                {
                    break;
                }

                int y = GET_Y_LPARAM(lParam);
                int index = y / self->itemHeight;

                if (index >= 0 && index < (int)self->activePopup->getItems().size())
                {
                    self->activePopup->getItems()[index].callback();
                }

                self->hide();

                SetFocus(self->parentHwnd);

                return 0;
            }

            case WM_KILLFOCUS:
            {
                if (self)
                {
                    self->hide();

                }
                return 0;
            }

            case WM_PAINT:
            {
                if (self)
                {
                    self->render();
                }
                ValidateRect(hwnd, nullptr);
                return 0;
            }
            }

            return DefWindowProc(hwnd, msg, wParam, lParam);
        }

        void createWindow()
        {
            WNDCLASS wc = {};
            wc.lpfnWndProc = WndProc;
            wc.hInstance = GetModuleHandle(nullptr);
            wc.lpszClassName = L"NBUI_POPUP_WINDOW";

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

            RECT rc;
            GetClientRect(popupHwnd, &rc);

            D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();

            D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(
                popupHwnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)
            );

            factory->CreateHwndRenderTarget(props, hwndProps, &popupRT);



            popupRT->CreateSolidColorBrush(
                D2D1::ColorF(1.f, 1.f, 1.f), // текст
                &brush
            );

            popupRT->CreateSolidColorBrush(
                D2D1::ColorF(0.2f, 0.2f, 0.2f), // hover
                &hoverBrush
            );

            popupRT->CreateSolidColorBrush(
                D2D1::ColorF(0.3f, 0.3f, 0.3f), // border
                &borderBrush
            );

            Microsoft::WRL::ComPtr<IDWriteFactory> writeFactory = Renderer::FactorySingleton::getDirectWriteFactory();

            writeFactory->CreateTextFormat(
                L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"ru-RU", &textFormat
            );

            textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        }

        void render() const
        {
            if (!popupRT || !activePopup || !textFormat || !brush)
            {
                return;
            }

            popupRT->BeginDraw();

            // Фон меню
            popupRT->Clear(D2D1::ColorF(0.10f, 0.10f, 0.10f));

            const float width = 180.0f;
            const float padding = 8.0f;

            int index = 0;

            for (const auto& item : activePopup->getItems())
            {
                float top = float(index * itemHeight);
                float bottom = float((index + 1) * itemHeight);

                D2D1_RECT_F rect = D2D1::RectF(0.0f, top, width, bottom);

                // 🔥 Hover подсветка
                if (index == hoveredIndex)
                {
                    popupRT->FillRectangle(
                        rect,
                        hoverBrush.Get() // сделай слегка светлее фона
                    );
                }

                // 🧠 Текстовый rect с padding
                D2D1_RECT_F textRect = rect;
                textRect.left += padding;

                popupRT->DrawTextW(
                    item.text.c_str(), (UINT32)item.text.size(), textFormat.Get(), textRect,
                    brush.Get()
                );

                index++;
            }

            // 🧱 Рамка (чтобы не выглядело как пятно)
            popupRT->DrawRectangle(
                D2D1::RectF(0.0f, 0.0f, width, float(index * itemHeight)), borderBrush.Get(), 1.0f
            );

            popupRT->EndDraw();
        }
    };

} // namespace nbui