
#ifndef NBUI_SRC_MAINWINDOW_HPP
#define NBUI_SRC_MAINWINDOW_HPP

#include <Windows.h>

#include <functional>
#include <bitset>
#include <string>

#include "Core.hpp"

#include "IWindow.hpp"
#include "ChildWindow.hpp"
#include "Utils.hpp"
#include "DoceTree.hpp" 

#include "Renderer/FactorySingleton.hpp"
#include "Renderer/Renderer.hpp"

#include "CaptionButtons.hpp"


// struct CaptionButton
// {
//     CaptionButton(const std::wstring& text, const int width, const int height = 30, const NbColor& hoverColor = {105, 105, 105})
//         : text(text)
//         , width(width)
//         , height(height)
//         , hoverColor(hoverColor)
//         , func()
//     {
//         rect.bottom = height;
//         rect.left = 0;
//         rect.right = width;
//         rect.top = 0;
//     }

//     void setFunc(std::function<void()> func)
//     {
//         this->func = func;
//     }

//     std::function<void()>   func;

//     RECT                    rect;
//     std::wstring            text;
//     int                     width;
//     int                     height      = 30;
//     NbColor                 hoverColor;
//     NbColor                 color;
// };



class MainWindow : public IWindow
{
public:
    MainWindow()
        : IWindow()
    {
        WNDCLASS wc = {};
        wc.lpfnWndProc = staticWndProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.lpszClassName = L"DarkThemeWindow";
        wc.style = CS_OWNDC;


        RegisterClass(&wc);
        size = { 500, 300 };

        HWND _handle = CreateWindow(wc.lpszClassName, L"Тёмная тема в WinAPI",
                              WS_OVERLAPPED | WS_THICKFRAME | WS_CLIPCHILDREN | WS_CLIPSIBLINGS , CW_USEDEFAULT, CW_USEDEFAULT, size.width, size.height,
                              nullptr, nullptr, wc.hInstance, nullptr);

        handle = NbWindowHandle::fromHwnd(_handle);

        DWORD style = GetWindowLong(handle, GWL_STYLE);
        style &= ~(WS_CAPTION | WS_THICKFRAME);
        SetWindowLong(handle, GWL_STYLE, style);
        
        initDirect2d();


        ShowWindow(handle, TRUE);
        UpdateWindow(handle);

        
        IWindow *w = new ChildWindow(handle, 100,40 );
        w->setBackgroundColor(NbColor(128, 128, 32));
        IWindow *w1 = new ChildWindow(handle, 200,200);
        w1->setBackgroundColor(NbColor(232, 232, 232));
        
        captionButtonColor = renderer.createSolidColorBrush(pRenderTarget, NbColor{ 105, 105, 105 });
        captionButtonsContainer = new CaptionButtons(handle);
        captionButtonRenderer = new CaptionButtonRenderer(pRenderTarget);
        //
        prevSize = size;

        position.x = CW_USEDEFAULT;
        position.y = CW_USEDEFAULT;

        prevPosition = position;

        captionButtons[0].setFunc([this]()
        {
            DestroyWindow(handle); 
        });
        captionButtons[1].setFunc([this]()
        {  
            if(IsMaximized(handle))
                ShowWindow(handle, SW_RESTORE);
            else
                ShowWindow(handle, SW_MAXIMIZE);
        });
        captionButtons[2].setFunc([this]()
        {
            CloseWindow(handle); 
        });

        captionButtonsContainer->addButton(captionButtons[0]);
        captionButtonsContainer->addButton(captionButtons[1]);
        captionButtonsContainer->addButton(captionButtons[2]);

        SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)this);


    }

    void onSizeChanged(const NbSize<int>& newSize) override
    {
        SafeRelease(&pRenderTarget);
        SafeRelease(&frameColor);
        SafeRelease(&backgroundColor);
        SafeRelease(&captionButtonColor);
        pRenderTarget = renderer.createHwndRenderTarget(handle, size);
        frameColor = renderer.createSolidColorBrush(pRenderTarget, NbColor{ 32, 32, 32 });
        backgroundColor = renderer.createSolidColorBrush(pRenderTarget, NbColor{ 46, 46, 46 });
        captionButtonColor = renderer.createSolidColorBrush(pRenderTarget, NbColor{ 105, 105, 105 });

        captionButtonsContainer->onSizeChanged();
        captionButtonRenderer->changeRenderTarget(pRenderTarget);

        InvalidateRect(handle, nullptr, FALSE);
    }

    void onClientPaint() override
    {
        pRenderTarget->BeginDraw();

        // Очистка клиентской области (фон)
        pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

        RECT rc;
        GetClientRect(handle, &rc);
        float width = static_cast<float>(rc.right - rc.left);
        float height = static_cast<float>(rc.bottom - rc.top);

        float borderThickness = 6.0f;
        float captionHeight = 30.0f;

        // Рисуем рамку (внутри клиентской области)
        D2D1_RECT_F borderRect = D2D1::RectF(0, 0, width, height);
        pRenderTarget->DrawRectangle(borderRect, frameColor, borderThickness);

        // Рисуем заголовок сверху
        D2D1_RECT_F captionRect = D2D1::RectF(0, 0, width, captionHeight);
        pRenderTarget->FillRectangle(captionRect, frameColor);

        captionButtonsContainer->draw(*captionButtonRenderer);

        HRESULT hr = pRenderTarget->EndDraw();
        if (FAILED(hr))
        {
            auto err = GetLastError();
        }

    }

    void onClientMouseMove(NbPoint<int> point) override
    {


    }

    void onClientMouseClick(NbPoint<int> point) override
    {
        
    }

    void onClientMouseLeave() override
    {

    }

    void onClientMouseRelease(NbPoint<int> point) override
    {
        
    }
    void onNonClientPaint(HRGN region) override
    {
        // Получаем HDC для всего окна (включая неклиентскую область)
        HDC hdc = GetWindowDC(handle);
        if (!hdc)
            return;

        // Получаем фабрику Direct2D
        ID2D1Factory* pFactory = Renderer::FactorySingleton::getFactory();

        // Свойства рендер-таргета с нужным форматом пикселей и альфа-режимом
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            0, 0, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT);

        ID2D1DCRenderTarget* pDCRenderTarget = nullptr;
        HRESULT hr = pFactory->CreateDCRenderTarget(&props, &pDCRenderTarget);
        if (SUCCEEDED(hr))
        {
            hr = pDCRenderTarget->BindDC(hdc, nullptr);
            if (SUCCEEDED(hr))
            {
                // Создаём кисть для рисования
                ID2D1SolidColorBrush* pBrush = nullptr;
                hr = pDCRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pBrush);
                if (SUCCEEDED(hr))
                {
                    pDCRenderTarget->BeginDraw();

                    // Получаем размеры окна в экранных координатах
                    RECT windowRect;
                    GetWindowRect(handle, &windowRect);

                    // Переводим координаты в клиентские (относительно окна)
                    POINT topLeft = { windowRect.left, windowRect.top };
                    POINT bottomRight = { windowRect.right, windowRect.bottom };
                    ScreenToClient(handle, &topLeft);
                    ScreenToClient(handle, &bottomRight);

                    // Вычисляем размер неклиентской области
                    D2D1_SIZE_F d2dSize = D2D1::SizeF(
                        static_cast<FLOAT>(bottomRight.x - topLeft.x),
                        static_cast<FLOAT>(bottomRight.y - topLeft.y));

                    // Очищаем область жёлтым цветом (можно заменить)
                    pDCRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Yellow));

                    // Рисуем верхнюю панель (например, заголовок)
                    D2D1_RECT_F upperFrame = D2D1::RectF(0, 0, d2dSize.width, 30.0f);
                    pDCRenderTarget->FillRectangle(upperFrame, pBrush);

                    hr = pDCRenderTarget->EndDraw();
                    if (FAILED(hr))
                    {
                        OutputDebugString(L"Failed to end draw\n");
                    }

                    pBrush->Release();
                }
            }
            pDCRenderTarget->Release();
        }

        ReleaseDC(handle, hdc);

        // Отмечаем, что окно обновлено
        ValidateRect(handle, nullptr);
    }



    
    void onNonClientHover(NbPoint<int> pos) override
    {
        int couter = 0;
        Utils::convertToWindowSpace(handle, pos);

        for(auto& button : captionButtons)
        {
            const auto &rect = button.rect;

            if(rect.bottom > pos.y && rect.top < pos.y 
                && rect.left < pos.x && rect.right > pos.x)
            {
                isCaptionButtonHovered[couter] = true;
            }
            else
            {
                isCaptionButtonHovered[couter] = false;
            }
            couter++;
        }
    }

    void onNonClientClick(NbPoint<int> pos) override
    {
        Utils::convertToWindowSpace(handle, pos);

        for(auto& button : captionButtons)
        {
            const auto &rect = button.rect;

            if(rect.bottom > pos.y && rect.top < pos.y 
                && rect.left < pos.x && rect.right > pos.x)
            {
                isCaptionButtonHeld[&button - &captionButtons[0]] = true;
            }
            else
            {
                isCaptionButtonHeld[&button - &captionButtons[0]] = false;
            }
        }
    }

    void onNonClientRelease(NbPoint<int> pos) override
    {
        Utils::convertToWindowSpace(handle, pos);

        for(auto& button : captionButtons)
        {
            const auto &rect = button.rect;

            if(rect.bottom > pos.y && rect.top < pos.y 
                && rect.left < pos.x && rect.right > pos.x)
            {
                if(isCaptionButtonHeld[&button - &captionButtons[0]])
                {
                    button.func();
                }
            }

            isCaptionButtonHeld[&button - &captionButtons[0]] = false;
        }
    }

    void onNonClientDoubleClick(NbPoint<int> pos)
    {
        if(IsMaximized(handle))
        {
            ShowWindow(handle, SW_RESTORE);
        }
        else
        {
            ShowWindow(handle, SW_MAXIMIZE);
        }
    }

    void onNonClientClickAndHold(NbPoint<int> pos) override
    {
        // Utils::convertToWindowSpace(handle, pos);

        // if(PtInRegion(frameRegion, pos.x, pos.y))
        // {
        //     frameColor = CreateSolidBrush(RGB(46, 46, 46));
        // }
        // else
        // {
        //     frameColor = CreateSolidBrush(RGB(100, 100, 100));
        // }
        
    }

    void resetNonClientState() override
    {
        isCaptionButtonHovered.reset();
        isCaptionButtonHeld.reset();
    }

    void onNonClientLeave() override
    {
        resetNonClientState();
        SendMessage(handle, WM_NCPAINT, 0, 0);
    }

    bool isInExcludedNonClientArea(NbPoint<int> pos) override
    {
        Utils::convertToWindowSpace(handle, pos);

        for(const auto& button : captionButtons)
        {
            const auto &rect = button.rect;
            if(rect.bottom > pos.y && rect.top < pos.y 
                && rect.left < pos.x && rect.right > pos.x)
            {
                return true;
            }
        }
        return false;
    }

private:

    //DoceTree *tree = nullptr;
    CaptionButtons* captionButtonsContainer = nullptr;
    CaptionButtonRenderer* captionButtonRenderer = nullptr;
    std::vector<CaptionButton>  captionButtons =
    {
        { L"✕", 50, 30, {240, 7, 23} },
        { L"🗖", 30, 30 },
        { L"🗕", 30, 30 },
    };

    ID2D1SolidColorBrush*       captionButtonColor      = nullptr;
    std::bitset<3>              isCaptionButtonHovered;
    std::bitset<3>              isCaptionButtonHeld;   
};

#endif