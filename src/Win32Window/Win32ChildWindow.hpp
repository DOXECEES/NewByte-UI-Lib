#ifndef NBUI_SRC_WIN32WINDOW_WIN32CHILDWINDOW_HPP
#define NBUI_SRC_WIN32WINDOW_WIN32CHILDWINDOW_HPP

#include "../WindowInterface/IWindow.hpp"

#include "Debug.hpp"
#include <windowsx.h>
#include <algorithm>

#include <Utility.hpp>
#include "Widgets/CheckBox.hpp"

#pragma comment(lib, "winmm.lib")

namespace Win32Window
{
    using namespace WindowInterface;
    
    class ChildWindow : public IWindow
    {
    public:
        constexpr static uint32_t   CARET_FLICKERING_TIME_MS = 500;

        ChildWindow(IWindow *parentWindow, bool setOwnDc = false);
        ~ChildWindow();

        void onSize(const NbSize<int>& newSize) override { };
        void show() override;
        void repaint() const noexcept override;

        const NbWindowHandle &getHandle() const noexcept { return handle; };

        void addCaption() noexcept;
        void setRenderable(bool flag) noexcept;
		bool getIsRenderable() const noexcept { return isRenderable; };

        inline static Widgets::IWidget* focusedWidget = nullptr; // only one widget can have focus

    public:
        Signal<void(const NbSize<int>&)> onSizeChanged;

        LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            static bool wasNonRenderable = false;
            static NbPoint<int> dragOffset = {};
            static bool dragging = false;
            static bool clicked = true;

            switch(message)
            {
                case WM_CREATE:
                {
                    timer = CreateThreadpoolTimer([](PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_TIMER Timer) {
                        HWND hwnd = reinterpret_cast<HWND>(Context);
                        InvalidateRect(hwnd, NULL, FALSE);

                        if (focusedWidget)
                        {
                            focusedWidget->onTimer();
                        }
                        
                        }, reinterpret_cast<PVOID>(hWnd), nullptr);

                    FILETIME dueTime;

                    ULONGLONG qwDueTime = -10000000LL;
                    dueTime.dwHighDateTime = (DWORD)(qwDueTime >> 32);
                    dueTime.dwLowDateTime = (DWORD)qwDueTime;

                    SetThreadpoolTimer(timer, &dueTime, CARET_FLICKERING_TIME_MS, 0);

                    //SetTimer(hWnd, 100, 500, nullptr);

                    return FALSE;
                }
                case WM_PAINT:
                {
                    PAINTSTRUCT ps;

                   /* if (!isRenderable)
                    {
                       HDC hdc = BeginPaint(hWnd, &ps);

                        EndPaint(hWnd, &ps);
                        return 0;
                    }*/

                    if(isRenderable)
                    {
                        state.title;
                        renderer->render(this);
                    }

                    ValidateRect(hWnd, nullptr);
            
                    return 0;
                } 
                case WM_SIZE:
                {
                    int xSize = LOWORD(lParam);
                    int ySize = HIWORD(lParam);
                    

                    state.setSize({xSize, ySize});
                    
                    //if (isRenderable == false)
                    //{
                    //    wasNonRenderable = true;
                    //    isRenderable = true;
                    //}


                    recalculateLayout();


                    if (renderer)
                    {
                        renderer->resize(this);
                        InvalidateRect(hWnd, nullptr, FALSE);

                    }

                    onSizeChanged.emit(state.size);
                    for (auto& listener : stateChangedListeners)
                    {
                        listener->onSizeChanged(state.clientSize);
                    }

                    return 0;
                }
                case WM_NCHITTEST:  // if's order important
                {
                    if(state.frameSize.isEmpty())
                        return HTCLIENT;

                    int x = GET_X_LPARAM(lParam);
                    int y = GET_Y_LPARAM(lParam);

                    POINT point = {x,y};

                    ScreenToClient(hWnd, &point);
                    
                    constexpr int SIZE_TO_MOVE_ARROW = 10;

                    if(point.x < SIZE_TO_MOVE_ARROW && point.y < SIZE_TO_MOVE_ARROW)
                        return HTTOPLEFT;

                    if (point.y < SIZE_TO_MOVE_ARROW)
                        return HTTOP;
                  
                    RECT rc;
                    GetClientRect(hWnd, &rc);
                  
                    if(point.y > rc.bottom - state.frameSize.bot && point.x < SIZE_TO_MOVE_ARROW)
                        return HTBOTTOMLEFT;

                    if(point.x > rc.right - state.frameSize.right && point.y > rc.bottom - state.frameSize.bot)
                        return HTBOTTOMRIGHT;

                    if (point.x < state.frameSize.left)
                        return HTLEFT;

                    if (point.x > rc.right - state.frameSize.right)
                        return HTRIGHT;

                    if (point.y < state.frameSize.bot) // same as bot 
                        return HTTOP;

                    if (point.y < state.frameSize.top)
                        return HTCAPTION;

                    if (point.y > rc.bottom - state.frameSize.bot)
                        return HTBOTTOM;

                    LoadCursor(nullptr, IDC_ARROW);
                    return HTCLIENT;
                }
                case WM_CHAR:
                {
                    if (!focusedWidget)
                    {
                        return FALSE;
                    }

                    focusedWidget->onSymbolButtonClicked(static_cast<wchar_t>(wParam));

                    return FALSE;
                }
                case WM_LBUTTONDOWN:
                {
                    SetCapture(hWnd);
                    NbPoint<int> point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

                    // Функция-помощник для получения ZIndex из узла лейаута
                    auto getZIndex = [](const NNsLayout::LayoutNode* node) -> Core::ZIndex {
                        if (auto widgetLayout = dynamic_cast<const NNsLayout::LayoutWidget*>(node)) {
                            if (auto widget = widgetLayout->getWidget().get()) {
                                return widget->getZIndex();
                            }
                        }
                        return Core::ZIndex(Core::ZIndex::ZType::MAIN, 0); // Для контейнеров без виджетов
                        };

                    // Лямбда для получения детей, отсортированных по ZIndex (сначала верхние)
                    auto getSortedChildren = [&](const NNsLayout::LayoutNode* node) {
                        nbstl::Vector<const NNsLayout::LayoutNode*> children;
                        int count = node->getChildrenSize();
                        children.reserve(count);
                        for (int i = 0; i < count; i++) {
                            children.pushBack(node->getChildrenAt(i));
                        }

                        // Сортируем детей по ZIndex. 
                        // Если ZIndex одинаковый, сохраняем порядок отрисовки (кто позже добавлен — тот выше)
                        std::stable_sort(children.begin(), children.end(), [&](const NNsLayout::LayoutNode* a, const NNsLayout::LayoutNode* b) {
                            return getZIndex(a) > getZIndex(b);
                            });

                        return children;
                        };

                    bool isFocusChanged = false;

                    // ПЕРВЫЙ ПРОХОД: Поиск виджета под курсором с учетом ZIndex
                    nbstl::dfs(
                        this->getLayoutRoot(),
                        getSortedChildren, // Используем сортировку здесь
                        [&](const NNsLayout::LayoutNode* node)
                        {
                            if (auto widgetLayout = dynamic_cast<const NNsLayout::LayoutWidget*>(node))
                            {
                                auto widget = widgetLayout->getWidget().get();
                                // hitTest должен проверять и внутренних детей (как мы писали для ComboBox)
                                if (widget && !widget->isHide() && widget->hitTest(point))
                                {
                                    isFocusChanged = true;
                                    if (focusedWidget && focusedWidget != widget) {
                                        focusedWidget->setUnfocused();
                                    }
                                    focusedWidget = widget;
                                    focusedWidget->setFocused();
                                    clicked = true;
                                    widget->onClick();
                                    return true; // Нашли самый верхний виджет, прерываем DFS
                                }
                            }
                            return false;
                        }
                    );

                    // ВТОРОЙ ПРОХОД: Логика клика (ComboBox toggle и т.д.)
                    nbstl::dfs(
                        this->getLayoutRoot(),
                        getSortedChildren, // И здесь тоже
                        [&](const NNsLayout::LayoutNode* node)
                        {
                            if (auto widgetLayout = dynamic_cast<const NNsLayout::LayoutWidget*>(node))
                            {
                                auto widget = widgetLayout->getWidget().get();
                                if (widget && !widget->isHide() && widget->hitTestClick(point))
                                {
                                    return true; // Событие обработано самым верхним виджетом
                                }
                            }
                            return false;
                        }
                    );

                    if (!isFocusChanged)
                    {
                        if (focusedWidget) {
                            focusedWidget->setUnfocused();
                            focusedWidget = nullptr;
                        }
                        // Закрываем все выпадающие списки, если кликнули по пустому месту
                        // (убедитесь, что метод объявлен как static в ComboBox.hpp)
                        //::Widgets::ComboBox::closeAllDropDowns();
                    }

                    InvalidateRect(hWnd, nullptr, FALSE);
                    return 0;
                }
                case WM_APP + 1:
                {
                    if (wasNonRenderable)
                    {
                        setRenderable(false);
                    }
                    Debug::debug("Resize finished");
                    return 0;
                }
                case WM_SETCURSOR:
                {
                    static HCURSOR defaultCursor = LoadCursor(NULL, IDC_ARROW);
                    if (LOWORD(lParam) == HTCLIENT)
                    {
                        SetCursor(defaultCursor);
                        return TRUE;
                    }
                    return DefWindowProc(hWnd, message, wParam, lParam);
                }
                case WM_MOUSEMOVE:
                {
                    NbPoint<int> point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

                    {
                        bool isHoveredSet = false;

                        std::vector<NNsLayout::LayoutNode*> stack;
                        stack.push_back(rootLayout.get());

                        while (!stack.empty())
                        {
                            auto node = stack.back();
                            stack.pop_back();

                            if (auto widgetNode = dynamic_cast<NNsLayout::LayoutWidget*>(node); widgetNode != nullptr)
                            {
                                if (auto widget = widgetNode->getWidget())
                                {
                                    if (widget->isDisable())
                                    {
                                        continue;
                                    }


                                    if (!isHoveredSet && widget->hitTest(point))
                                    {
                                        widget->setHover();
                                        isHoveredSet = true;
                                    }
                                    else
                                    {
                                        widget->setDefault();
                                    }
                                }
                            }
                            

                            for (auto& child : node->getChildren())
                            {
                                stack.push_back(child.get());
                            }
                        }


                    }

                    InvalidateRect(hWnd, NULL, FALSE);
                    return 0;
                }
                case WM_KEYDOWN:
                {
                    if (!focusedWidget)
                        return 0;

                    if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
                    {
                        focusedWidget->onButtonClicked(wParam, SpecialKeyCode::CTRL);
                    }
                    else
                    {
                        focusedWidget->onButtonClicked(wParam);
                    }

                    InvalidateRect(hWnd, nullptr, FALSE);

                    return 0;
                }
                case WM_TIMER:
                {
                    if (focusedWidget != nullptr)
                    {
                        focusedWidget->onTimer();
                    }
                    InvalidateRect(hWnd, NULL, FALSE);
                    //UpdateWindow(hWnd);

                    return FALSE;
                }
                case WM_LBUTTONUP:
                {
                    ReleaseCapture();
                    dragging = false;
                 
                    NbPoint<int> point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

                    
                    nbstl::dfs(
                        this->getLayoutRoot(),
                        [](const NNsLayout::LayoutNode* node)
                        {
                            nbstl::Vector<const NNsLayout::LayoutNode*> children;
                            children.reserve(node->getChildrenSize());
                            for (int i = 0; i < node->getChildrenSize(); i++)
                            {
                                children.pushBack(node->getChildrenAt(i));
                            }
                            return children;
                        },
                        [&](const NNsLayout::LayoutNode* node)
                        {
                            if (auto widgetLayout = dynamic_cast<const NNsLayout::LayoutWidget*>(node))
                            {
                                auto widget = widgetLayout->getWidget().get();
                                if (widget && !widget->isHide() && widget->hitTest(point))
                                {
                                    widget->onRelease();
                                    return true;
                                }
                            }
                            return false;
                        }
                    );
                    
                    
                    return 0;
                }
                case WM_GETMINMAXINFO:
                {
                    LPMINMAXINFO mmi = (LPMINMAXINFO)lParam;
                    mmi->ptMinTrackSize.x = state.minSize.width;
                    mmi->ptMinTrackSize.y = state.minSize.height;
                    return 0;
                }
                case WM_ERASEBKGND:
                {
                    return 1;
                }

            }
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        inline static LRESULT CALLBACK staticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            ChildWindow *pThis = reinterpret_cast<ChildWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
            
            if (message == WM_NCCREATE)
            {
                auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
                pThis = static_cast<ChildWindow*>(cs->lpCreateParams);
                SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
            }

            if (pThis)
                return pThis->wndProc(hWnd, message, wParam, lParam);
            return DefWindowProc(hWnd, message, wParam, lParam);

                    
        }
    private:
        bool isRenderable = true;
        PTP_TIMER           timer;

    };
};

#endif//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////