// #ifndef NBUI_SRC_CHILDWINDOW_HPP
// #define NBUI_SRC_CHILDWINDOW_HPP

// #include <Windows.h>

// #include "IWindow.hpp"
// #include "Utils.hpp" 
// #include "Renderer/FactorySingleton.hpp"
// #include "Renderer/Renderer.hpp"

// #include <d2d1.h>
// #pragma comment(lib, "d2d1")


// class ChildWindow : public IWindow
// {
// public:
//    ChildWindow(HWND _handle, int x, int y)
//     {
//         WNDCLASS wc = {};
//         wc.lpfnWndProc = staticWndProc;
//         wc.hInstance = GetModuleHandle(nullptr);
//         wc.hbrBackground = nullptr;
//         wc.style = CS_OWNDC;
//         wc.lpszClassName = L"ChildWindow";
//         RegisterClass(&wc);

//         size = { 400, 300 };

//         HWND ihandle = CreateWindowEx(0, wc.lpszClassName, L"Child Window",
//             WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
//             x, y, size.width, size.height, _handle, nullptr, wc.hInstance, nullptr);

//         handle = NbWindowHandle::fromHwnd(ihandle);

//         initDirect2d();


//         SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)this);
//         setFrameSize({ 0, 0, 0, 0 });



//         // Получение размеров окна
//         RECT rc;
//         GetClientRect(handle, &rc);

//         //pRenderTarget = renderer.createHwndRenderTarget(handle, { rc.right - rc.left, rc.bottom - rc.top });
//         if(pRenderTarget == nullptr)
//         {
//             OutputDebugString(L"Failed to create render target");
//             return;
//         }

//         pRenderTarget->CreateSolidColorBrush(
//             D2D1::ColorF(D2D1::ColorF::DarkMagenta), &pHeaderBrush);

//         pRenderTarget->CreateSolidColorBrush(
//             D2D1::ColorF(D2D1::ColorF::DimGray), &backgroundColor);

//         ShowWindow(handle, TRUE);
//         UpdateWindow(handle);
//     }

//     void onClientPaint()
//     {
//         if (!pRenderTarget) return;

//         RECT r;
//         GetClientRect(handle, &r);
//         D2D1_RECT_F rect = D2D1::RectF((FLOAT)r.left, (FLOAT)r.top, (FLOAT)r.right, (FLOAT)r.bottom);

//         pRenderTarget->BeginDraw();
//         pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

//         // Фон
//         pRenderTarget->FillRectangle(rect, backgroundColor);

//         // Хедер
//         headerRect = RECT{ 0, 0, r.right, 30 };
//         D2D1_RECT_F header = D2D1::RectF(0, 0, (FLOAT)r.right, 30.0f);
//         pRenderTarget->FillRectangle(header, pHeaderBrush);

//         HRESULT hr = pRenderTarget->EndDraw();
//         if (FAILED(hr)) {
//             // Обработка ошибок
//         }
//     }


//     void onClientMouseMove(NbPoint<int> point) override 
//     {
//         static NbPoint<int> prevMousePos = { 0, 0 };
//         if (prevMousePos == point) return;

//         if (isLmbHolds && GetCapture() == handle)
//         {

//             if(!IsWindow(handle)) {
//                 return;
//             }

//         // Получаем текущие координаты окна
//             RECT windowRect;
//             if(!GetWindowRect(handle, &windowRect)) {
//                 return;
//             }
            
//             // Преобразуем экранные координаты в координаты родителя
//             POINT parentPos = { windowRect.left, windowRect.top };
//             ScreenToClient(GetParent(handle), &parentPos);

//             RECT parentRect;
//             GetWindowRect(GetParent(handle), &parentRect);

//             // Смещение относительно начальной точки захвата
//             int dx = point.x - dragStartPosition.x;
//             int dy = point.y - dragStartPosition.y;
            
//             // Новая позиция окна
//             int newX = parentPos.x + dx;
//             int newY = parentPos.y + dy;

//             RECT parentClientRect;
//             GetClientRect(GetParent(handle), &parentClientRect);

//             // Ограничиваем перемещение в пределах родителя
//             newX = max(0, min(newX, parentClientRect.right - (windowRect.right - windowRect.left)));
//             newY = max(0, min(newY, parentClientRect.bottom - (windowRect.bottom - windowRect.top)));

//             // Плавное перемещение
//             SetWindowPos(
//                 handle, 
//                 nullptr, 
//                 newX, 
//                 newY, 
//                 0, 
//                 0, 
//                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER
//             );
//             // 
//             RECT updateRect;
//             GetWindowRect(handle, &updateRect);
//             MapWindowPoints(nullptr, GetParent(handle), (POINT*)&updateRect, 2);

//             RedrawWindow(
//                 GetParent(handle),
//                 &updateRect,
//                 nullptr,
//                 RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW
//             );


//             //RedrawWindow(GetParent(handle), nullptr, nullptr, RDW_INVALIDATE | RDW_ALLCHILDREN);

//         }
//         else
//         {
//             SetCursor(LoadCursor(nullptr, IDC_ARROW));
//         }


//         prevMousePos = point;
//     }

//     void onClientMouseLeave() override
//     {
//         SetCursor(LoadCursor(nullptr, IDC_ARROW));    
//         headerTracking = false;
//     }

//     void onClientMouseClick(NbPoint<int> pos) override 
//     {
        
//         if (!IsWindow(handle)) return;

//         static NbPoint<int> prevMousePos = { 0, 0 };
//         if(prevMousePos == pos) return;
        
//         if (Utils::isPointInsideRect(pos, headerRect))
//         {
//             isLmbHolds = true;
//             HWND hPrevCapture = SetCapture(handle);
//             if (hPrevCapture == NULL && GetLastError() != 0) 
//             {
//                 DWORD err = GetLastError();
//                 OutputDebugStringA(("SetCapture failed: " + std::to_string(err)).c_str());
//             }
            
//             dragStartPosition = pos;
//         }

//         SetWindowPos(handle, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
//         //InvalidateRect(handle, NULL, TRUE);
//         //UpdateWindow(handle);
//     };

//     void onClientMouseRelease(NbPoint<int> pos) override
//     {
//         if (isLmbHolds)
//         {
//             isLmbHolds = false;
//             ReleaseCapture(); // Освобождаем мышь
//         }

//     }



//     void onNonClientPaint(HRGN region)                   
//     {
//         // HDC hdc = GetDCEx(handle, region, DCX_WINDOW | DCX_INTERSECTRGN | DCX_USESTYLE);
//         // if (!hdc) return; 
//         // RECT rc;
//         // GetWindowRect(handle, &rc);

//         // SIZE size = SIZE(rc.right - rc.left, rc.bottom - rc.top);

//         // RECT newRect = RECT(0, 0, size.cx, size.cy);
        
//         // if (frameRegion) DeleteObject(frameRegion);
//         // frameRegion = CreateRectRgn(0, 0, size.cx, size.cy);
    

//         // FillRect(hdc, &newRect, frameColor);

//         // ReleaseDC(handle, hdc);
//     };
//     void onNonClientClick(NbPoint<int> pos)              
//     {
//         OutputDebugString(L"Child got nonClient\n");
//     };

//     void onNonClientClickAndHold(NbPoint<int> pos)       
//     {
//         OutputDebugString(L"Child got WM_LBUTTONDOWN\n");

//         Utils::convertToWindowSpace(handle, pos);

//         // if(PtInRegion(frameRegion, pos.x, pos.y))
//         // {
//         //     frameColor = CreateSolidBrush(RGB(46, 46, 46));
//         // }
//         // else
//         // {
//         //     frameColor = CreateSolidBrush(RGB(100, 100, 100));
//         // }
//     };
//     void onNonClientHover(NbPoint<int> pos)              {};
//     void onNonClientDoubleClick(NbPoint<int> pos)        {};
//     void onNonClientRelease(NbPoint<int> pos)            {};
//     void resetNonClientState()                           {}; 
//     void onNonClientLeave()                              {};
//     void onSizeChanged(const NbSize<int>& newSize)       {};
//     bool isInExcludedNonClientArea(NbPoint<int> pos)    { return true; };


//     ~ChildWindow()
//     {
//         if (pHeaderBrush) pHeaderBrush->Release();

//         if (headerColor) DeleteObject(headerColor);
//         if (backgroundColor) DeleteObject(backgroundColor);
//         if (frameColor) DeleteObject(frameColor);
//         if (frameRegion) DeleteObject(frameRegion);
//     }


// private:
//     ID2D1SolidColorBrush* pHeaderBrush = nullptr;

//     HBRUSH headerColor = CreateSolidBrush(RGB(120, 34, 92)); // оставлено для других мест
//     RECT headerRect;
//     NbPoint<int> dragStartPosition = { 0, 0 };
//     bool headerTracking = false;

// };

// #endif