#ifndef NBUI_SRC_WIN32WINDOW_WIN32CHILDWINDOW_HPP
#define NBUI_SRC_WIN32WINDOW_WIN32CHILDWINDOW_HPP

#include "../WindowInterface/IWindow.hpp"

#include "Debug.hpp"
#include <windowsx.h>
#include <algorithm>

#include <Utility.hpp>
#include <winuser.h>
#include "Widgets/CheckBox.hpp"
#include "Layout/LayoutWidget.hpp"

#include "GlobalWidgetContext.hpp"

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

        void onMouseWheel(int delta) override
        {
            // 1. Получаем корень разметки
            auto* root = this->getLayoutRoot();
            if (!root)
            {
                return;
            }

            // 2. Находим наш VLayout (он обычно первый или единственный ребенок корня)
            // В вашем коде: Window -> LayoutRoot -> VLayout (финальный UI)
            auto& children = root->getChildren();
            if (children.empty())
            {
                return;
            }

            // Нам нужен именно VLayout, в котором лежат компоненты
            auto* vLayout = dynamic_cast<NNsLayout::VLayout*>(children[0].get());
            if (!vLayout)
            {
                return;
            }

            // 3. Вычисляем границы скролла
            int contentHeight = vLayout->getMeasuredSize().height; // Полная высота всех полей
            int viewHeight = vLayout->getRect().height;            // Высота видимого окна
            int maxScroll = std::max(0, contentHeight - viewHeight);

            // 4. Обновляем смещение (delta обычно +1 или -1, умножаем на скорость скролла)
            int currentOffset = vLayout->getScrollOffset();
            int scrollSpeed = 30; // Пикселей за один щелчок колеса
            int newOffset = std::clamp(currentOffset - (delta * scrollSpeed), 0, maxScroll);

            // 5. Применяем и помечаем разметку как "грязную", чтобы она пересчиталась
            if (newOffset != currentOffset)
            {
                vLayout->setScrollOffset(newOffset);
                vLayout->markDirty();
            }
        
        }




        void onSize(const NbSize<int>& newSize) override { };
        void show() override;
        void repaint() const noexcept override;

        const NbWindowHandle &getHandle() const noexcept { return handle; };

        void addCaption() noexcept;
        void setRenderable(bool flag) noexcept;
		bool getIsRenderable() const noexcept { return isRenderable; };
        void setSize(const NbSize<int>& newSize)
        {
            state.setSize(newSize);
            SetWindowPos(handle.as<HWND>(), nullptr, 0, 0, state.size.width, state.size.height, SWP_NOMOVE | SWP_NOOWNERZORDER) ;
        }

        void setPosition(const NbPoint<int>& point)
        {
            SetWindowPos(
                handle.as<HWND>(), nullptr, point.x, point.y, 0, 0,
                SWP_NOSIZE | SWP_NOOWNERZORDER
            );
        }

        inline static Widgets::IWidget* focusedWidget = nullptr; // only one widget can have focus


        Widgets::IWidget* hitTestRecursive(
            NNsLayout::LayoutNode* node,
            const NbPoint<int>& point
        )
        {
            // 1. Сначала дети (в обратном порядке)
            for (auto it = node->getChildren().rbegin(); it != node->getChildren().rend(); ++it)
            {
                if (auto result = hitTestRecursive(it->get(), point))
                {
                    return result;
                }
            }

            // 2. Потом сам узел
            if (auto widgetNode = dynamic_cast<NNsLayout::LayoutWidget*>(node))
            {
                if (auto widget = widgetNode->getWidget())
                {
                    if (!widget->isDisable() && !widget->isHoverStateDisable() &&
                        widget->hitTest(point))
                    {
                        return widget.get();
                    }
                }
            }

            return nullptr;
        }


        void close() override
        {
            if (handle.as<HWND>())
            {
                PostMessage(handle.as<HWND>(), WM_CLOSE, 0, 0);
            }
        }

        bool isMouseCurrentlyDragging()
        {
            return isMouseDragging;
        }

        const NbPoint<int> getMouseCapturePoint()
        {
            return mouseCapturePoint;
        }

    public:
        Signal<void(const NbSize<int>&)> onSizeChanged;
        Signal<void()> onDraw;
        NbPoint<int> prevMousePoint = {-1, -1};
        NbPoint<int> mousePosition = {0, 0};
        bool leftMouseClicked = false;


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
                            //focusedWidget->onTimer();
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
                    recalculateLayout();
                    if(isRenderable)
                    {
                        renderer->render(this);
                    }

                    onDraw.emit();
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
                case WM_MOUSEWHEEL:
                {
                    int   delta = GET_WHEEL_DELTA_WPARAM(wParam);
                    POINT pt;
                    pt.x = GET_X_LPARAM(lParam);
                    pt.y = GET_Y_LPARAM(lParam);
                    ScreenToClient(hWnd, &pt);
                    NbPoint<int> mousePoint = {(int)pt.x, (int)pt.y};

                    NNsLayout::VLayout* targetScrollLayout = nullptr;

                    nbstl::dfs(
                        this->getLayoutRoot(),
                        [](const NNsLayout::LayoutNode* node)
                        {
                            nbstl::Vector<const NNsLayout::LayoutNode*> children;
                            int count = node->getChildrenSize();
                            children.reserve(count);
                            for (int i = 0; i < count; i++)
                            {
                                children.pushBack(node->getChildrenAt(i));
                            }
                            return children;
                        },
                        [&](const NNsLayout::LayoutNode* node)
                        {
                            auto vLayout = dynamic_cast<const NNsLayout::VLayout*>(node);
                            if (vLayout)
                            {
                                const auto& rect = vLayout->getRect();
                                if (mousePoint.x >= rect.x && mousePoint.x <= rect.x + rect.width &&
                                    mousePoint.y >= rect.y && mousePoint.y <= rect.y + rect.height)
                                {
                                    targetScrollLayout = const_cast<NNsLayout::VLayout*>(vLayout);
                                    return true;
                                }
                            }
                            return false;
                        }
                    );

                    if (targetScrollLayout)
                    {
                        int contentHeight = 0;

                        int count = targetScrollLayout->getChildrenSize();
                        for (int i = 0; i < count; i++)
                        {
                            auto child = targetScrollLayout->getChildrenAt(i);
                            if (child)
                            {
                                contentHeight += child->getRect().height;
                            }
                        }

                        int viewHeight = targetScrollLayout->getRect().height;

                        int maxScroll = (std::max)(0, contentHeight - viewHeight);

                        int currentOffset = targetScrollLayout->getScrollOffset();

                        int scrollStep   = 40;
                        int scrollAmount = (delta / WHEEL_DELTA) * scrollStep;

                        int newOffset = (std::clamp)(currentOffset - scrollAmount, 0, maxScroll);

                        if (newOffset != currentOffset)
                        {
                            targetScrollLayout->setScrollOffset(newOffset);
                            targetScrollLayout->markDirty();

                            InvalidateRect(hWnd, NULL, FALSE);
                        }
                    }
                    return 0;
                }
                case WM_LBUTTONDOWN:
                {
                    SetFocus(hWnd);
                    SetCapture(hWnd);
                    isMouseDragging = true;
                    leftMouseClicked = true;
                    NbPoint<int> point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                    mouseCapturePoint = point;

                    auto getZIndex = [](const NNsLayout::LayoutNode* node) -> Core::ZIndex
                    {
                        if (auto widgetLayout = dynamic_cast<const NNsLayout::LayoutWidget*>(node))
                        {
                            if (auto widget = widgetLayout->getWidget().get())
                            {
                                return widget->getZIndex();
                            }
                        }
                        return Core::ZIndex(Core::ZIndex::ZType::MAIN, 0);
                    };

                    auto getSortedChildren = [&](const NNsLayout::LayoutNode* node)
                    {
                        nbstl::Vector<const NNsLayout::LayoutNode*> children;
                        int count = node->getChildrenSize();
                        children.reserve(count);
                        for (int i = 0; i < count; i++)
                        {
                            children.pushBack(node->getChildrenAt(i));
                        }
                        std::stable_sort(
                            children.begin(), children.end(),
                            [&](const NNsLayout::LayoutNode* a, const NNsLayout::LayoutNode* b)
                            {
                                return getZIndex(a) > getZIndex(b);
                            }
                        );
                        return children;
                    };

                    std::function<::Widgets::IWidget*(::Widgets::IWidget*, NbPoint<int>, int)>
                        findDeepestWidget;
                    findDeepestWidget = [&](::Widgets::IWidget* current, NbPoint<int> p,
                                            int depth = 0) -> ::Widgets::IWidget*
                    {
                        if (depth > 100)
                        {
                            return current; // Защита от зависания
                        }
                        const auto& subChildren = current->getChildrens();
                        for (auto it = subChildren.rbegin(); it != subChildren.rend(); ++it)
                        {
                            auto* sub = it->get();
                            if (sub && sub != current && !sub->isHide() && !sub->isDisable() &&
                                sub->hitTest(p))
                            {
                                return findDeepestWidget(sub, p, depth + 1);
                            }
                        }
                        return current;
                    };


                    bool isFocusChanged = false;
                    ::Widgets::IWidget* clickedTarget = nullptr;

                    nbstl::dfs(
                        this->getLayoutRoot(), getSortedChildren,
                        [&](const NNsLayout::LayoutNode* node)
                        {
                            if (auto widgetLayout =
                                    dynamic_cast<const NNsLayout::LayoutWidget*>(node))
                            {
                                auto rootWidget = widgetLayout->getWidget().get();
                                if (rootWidget && !rootWidget->isHide() &&
                                    rootWidget->hitTest(point))
                                {
                                    clickedTarget = findDeepestWidget(rootWidget, point, 0);
                                    if (clickedTarget)
                                    {
                                        isFocusChanged = true;
                                        if (focusedWidget && focusedWidget != clickedTarget)
                                        {
                                            focusedWidget->setUnfocused();
                                            //nbui::GlobalWidgetContext::onUnfocus();
                                        }
                                        focusedWidget = clickedTarget;
                                        focusedWidget->setFocused();
                                        nbui::GlobalWidgetContext::captureFocusedWidget(focusedWidget);
                                        clicked = true;
                                        return true; 
                                    }
                                }
                            }
                            return false;
                        }
                    );

                    bool clickHandled = false;
                    nbstl::dfs(
                        this->getLayoutRoot(), getSortedChildren,
                        [&](const NNsLayout::LayoutNode* node)
                        {
                            if (auto widgetLayout =
                                    dynamic_cast<const NNsLayout::LayoutWidget*>(node))
                            {
                                auto widget = widgetLayout->getWidget().get();
                                if (widget && !widget->isHide() && widget->hitTestClick(point))
                                {
                                    clickHandled = true;
                                    return true;
                                }
                            }
                            return false;
                        }
                    );

                    if (!clickHandled && clickedTarget)
                    {
                        clickedTarget->onClick();
                    }

                    nbui::GlobalWidgetContext::capturePressedWidget(clickedTarget);

                    if (!isFocusChanged)
                    {
                        if (focusedWidget)
                        {
                            focusedWidget->setUnfocused();
                            focusedWidget = nullptr;
                            //nbui::GlobalWidgetContext::onUnfocus();
                            nbui::GlobalWidgetContext::releaseFocusedWidget();

                            
                        }
                        // ::Widgets::Menu::closeAllMenu()
                        //::Widgets::ComboBox::closeAllDropDowns();
                    }

                    InvalidateRect(hWnd, nullptr, FALSE);
                    return 0;
                }
                case WM_RBUTTONDOWN:
                {
                    SetFocus(hWnd);

                    NbPoint<int> point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};


                    auto getZIndex = [](const NNsLayout::LayoutNode* node) -> Core::ZIndex
                    {
                        if (auto widgetLayout = dynamic_cast<const NNsLayout::LayoutWidget*>(node))
                        {
                            if (auto widget = widgetLayout->getWidget().get())
                            {
                                return widget->getZIndex();
                            }
                        }
                        return Core::ZIndex(Core::ZIndex::ZType::MAIN, 0);
                    };

                    auto getSortedChildren = [&](const NNsLayout::LayoutNode* node)
                    {
                        nbstl::Vector<const NNsLayout::LayoutNode*> children;
                        int count = node->getChildrenSize();
                        children.reserve(count);

                        for (int i = 0; i < count; i++)
                        {
                            children.pushBack(node->getChildrenAt(i));
                        }

                        std::stable_sort(
                            children.begin(), children.end(),
                            [&](const NNsLayout::LayoutNode* a, const NNsLayout::LayoutNode* b)
                            {
                                return getZIndex(a) > getZIndex(b);
                            }
                        );

                        return children;
                    };

                    std::function<::Widgets::IWidget*(::Widgets::IWidget*, NbPoint<int>)>
                        findDeepestWidget;

                    findDeepestWidget = [&](::Widgets::IWidget* current,
                                            NbPoint<int> p) -> ::Widgets::IWidget*
                    {
                        const auto& subChildren = current->getChildrens();

                        for (auto it = subChildren.rbegin(); it != subChildren.rend(); ++it)
                        {
                            auto* sub = it->get();

                            if (sub && !sub->isHide() && !sub->isDisable() && sub->hitTest(p))
                            {
                                return findDeepestWidget(sub, p);
                            }
                        }

                        return current;
                    };


                    ::Widgets::IWidget* target = nullptr;

                    nbstl::dfs(
                        this->getLayoutRoot(), getSortedChildren,
                        [&](const NNsLayout::LayoutNode* node)
                        {
                            if (auto widgetLayout =
                                    dynamic_cast<const NNsLayout::LayoutWidget*>(node))
                            {
                                auto rootWidget = widgetLayout->getWidget().get();

                                if (rootWidget && !rootWidget->isHide() &&
                                    rootWidget->hitTest(point))
                                {
                                    target = findDeepestWidget(rootWidget, point);

                                    if (target)
                                    {
                                        return true; 
                                    }
                                }
                            }

                            return false;
                        }
                    );

                    if (target)
                    {
                        if (target->hitTestRightClick(point))
                        {
                            target->onRightClick(point);
                        }
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
                    mousePosition = point;
                    NbPoint<float> floatPoint = {(float)point.x, (float)point.y};
                    onMouseMove.emit(floatPoint);

                    MouseState mouseState = {.position = point};

                    if (prevMousePoint.x == -1 && prevMousePoint.y == -1)
                    {
                        prevMousePoint.x = point.x;
                        prevMousePoint.y = point.y;
                    }

                    int dx = point.x - prevMousePoint.x;
                    int dy = point.y - prevMousePoint.y;

                    if (dx > 0)
                    {
                        mouseState.direction = MouseDirection::RIGHT;
                    }
                    else if (dx < 0)
                    {
                        mouseState.direction = MouseDirection::LEFT;
                    }

                    if (dy > 0)
                    {
                        mouseState.direction = MouseDirection::DOWN;
                    }
                    else if (dy < 0)
                    {
                        mouseState.direction = MouseDirection::UP;
                    }

                    prevMousePoint = point;

                    {
                        nbui::GlobalWidgetContext::onMouseMove(mouseState);
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
                                    for (auto& w : widget->getChildrens())
                                    {
                                        if (w->isHide())
                                        {
                                            continue;
                                        }
                                        if (w->isDisable())
                                        {
                                            continue;
                                        }

                                        if (!w->isHoverStateDisable() && !isHoveredSet &&
                                            w->hitTest(point))
                                        {
                                            w->setHover();
                                            isHoveredSet = true;
                                            break;
                                        }
                                        else
                                        {
                                            w->setDefault();
                                        }
                                    }


                                    if (widget->isDisable())
                                    {
                                        continue;
                                    }


                                    if (!widget->isHoverStateDisable() && !isHoveredSet && widget->hitTest(point))
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
                    isMouseDragging = false;
                    mouseCapturePoint = {-1, -1};
                    dragging = false;
                    leftMouseClicked = false;

                 
                    NbPoint<int> point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

                    if (auto pressedWidget = nbui::GlobalWidgetContext::getPressedWidget();
                        pressedWidget)
                    {
                        pressedWidget->onRelease();
                        nbui::GlobalWidgetContext::releasePressedWidget();
                        return 0;
                    }
                    
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
                                    for (auto child : widget->getChildrens())
                                    {
                                        if (child->hitTest(point))
                                        {
                                            child->onRelease();
                                            return true;
                                        }
                                    }
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
                case WM_CLOSE:
                {
                    DestroyWindow(hWnd);
                    return 0;
                }
                case WM_DESTROY:
                {
                    if (timer)
                    {
                        SetThreadpoolTimer(timer, NULL, 0, 0);        
                        WaitForThreadpoolTimerCallbacks(timer, TRUE); 
                        CloseThreadpoolTimer(timer);                  
                        timer = nullptr;
                    }

                    if (focusedWidget)
                    {
                        focusedWidget = nullptr;
                    }

                    onClose.emit();

                    return 0;
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
        bool isMouseDragging = false;
        NbPoint<int> mouseCapturePoint = {-1, -1};
        bool isRenderable = true;
        PTP_TIMER           timer;

    };
};

#endif//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////