#ifndef NBUI_SRC_WIN32WINDOW_WIN32MODALWINDOW_HPP
#define NBUI_SRC_WIN32WINDOW_WIN32MODALWINDOW_HPP

#include <Windows.h>
#include <windowsx.h>

#include "../WindowInterface/WindowCore.hpp"

#include "../WindowInterface/IWindow.hpp"
#include "../WindowInterface/WindowMapper.hpp"
#include "Core.hpp"
#include "Layout/LayoutWidget.hpp"
#include "Signal.hpp"

#include <Utility.hpp>
#include <algorithm>

#include <GlobalWidgetContext.hpp>

namespace Win32Window
{
    class ModalWindow : public WindowInterface::IWindow
    {
    public:
        ModalWindow(const NbSize<int>& size, IWindow* parent);
        ~ModalWindow();

        void show() override;
        void repaint() const noexcept override;

        void close() override
        {
            if (handle.as<HWND>())
            {
                SendMessage(handle.as<HWND>(), WM_CLOSE, 0, 0);
            }
        }

        void onSize(const NbSize<int>& newSize) override
        {
            OutputDebugString(L"Window resized\n");
            state.setSize(newSize);

            captionButtonsContainer.setPaintArea(NbRect<int>(0, 0, state.size.width, state.size.height));
            //renderer->resize(this);
        };

    public:
        Signal<void(const NbSize<int>&)> onSizeChanged;
    private:
        inline static Widgets::IWidget* focusedWidget = nullptr; // only one widget can have focus

        IWindow* parent = nullptr;
        NbPoint<int> prevMousePoint = {-1, -1};


        LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            static bool wasNonRenderable = false;
            static NbPoint<int> dragOffset = {};
            static bool dragging = false;
            static bool clicked = true;

            switch (message)
            {
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
                
                renderer->render(this);
                

                ValidateRect(hWnd, nullptr);

                return 0;
            }
            case WM_SIZE:
            {
                int xSize = LOWORD(lParam);
                int ySize = HIWORD(lParam);

                state.setSize({xSize, ySize});

                // if (isRenderable == false)
                //{
                //     wasNonRenderable = true;
                //     isRenderable = true;
                // }

                recalculateLayout();

                if (renderer)
                {
                    renderer->resize(this);
                    InvalidateRect(hWnd, nullptr, FALSE);
                }

                onSizeChanged.emit(state.clientSize);
                for (auto& listener : stateChangedListeners)
                {
                    listener->onSizeChanged(state.clientSize);
                }

                return 0;
            }
            case WM_NCHITTEST: // if's order important
            {
                if (state.frameSize.isEmpty())
                {
                    return HTCLIENT;
                }

                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);

                POINT point = {x, y};

                ScreenToClient(hWnd, &point);

                constexpr int SIZE_TO_MOVE_ARROW = 10;

                if (point.x < SIZE_TO_MOVE_ARROW && point.y < SIZE_TO_MOVE_ARROW)
                {
                    return HTTOPLEFT;
                }

                if (point.y < SIZE_TO_MOVE_ARROW)
                {
                    return HTTOP;
                }

                RECT rc;
                GetClientRect(hWnd, &rc);

                if (point.y > rc.bottom - state.frameSize.bot && point.x < SIZE_TO_MOVE_ARROW)
                {
                    return HTBOTTOMLEFT;
                }

                if (point.x > rc.right - state.frameSize.right &&
                    point.y > rc.bottom - state.frameSize.bot)
                {
                    return HTBOTTOMRIGHT;
                }

                if (point.x < state.frameSize.left)
                {
                    return HTLEFT;
                }

                if (point.x > rc.right - state.frameSize.right)
                {
                    return HTRIGHT;
                }

                if (point.y < state.frameSize.bot) // same as bot
                {
                    return HTTOP;
                }

                if (point.y < state.frameSize.top)
                {
                    return HTCAPTION;
                }

                if (point.y > rc.bottom - state.frameSize.bot)
                {
                    return HTBOTTOM;
                }

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

                    // --- 1. ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ---

                    // Получение ZIndex из узла
                    auto getZIndex = [](const NNsLayout::LayoutNode* node) -> Core::ZIndex {
                        if (auto widgetLayout = dynamic_cast<const NNsLayout::LayoutWidget*>(node)) {
                            if (auto widget = widgetLayout->getWidget().get()) {
                                return widget->getZIndex();
                            }
                        }
                        return Core::ZIndex(Core::ZIndex::ZType::MAIN, 0);
                    };

                    // Сортировка детей по ZIndex (от верхних к нижним)
                    auto getSortedChildren = [&](const NNsLayout::LayoutNode* node) {
                        nbstl::Vector<const NNsLayout::LayoutNode*> children;
                        int count = node->getChildrenSize();
                        children.reserve(count);
                        for (int i = 0; i < count; i++) {
                            children.pushBack(node->getChildrenAt(i));
                        }
                        std::stable_sort(children.begin(), children.end(), [&](const NNsLayout::LayoutNode* a, const NNsLayout::LayoutNode* b) {
                            return getZIndex(a) > getZIndex(b); // Верхние первыми
                        });
                        return children;
                    };

                    // РЕКУРСИВНЫЙ ПОИСК самого глубокого под-виджета внутри найденного Widget
                    // Позволяет найти кнопку внутри ButtonGroup или элемент внутри контейнера
                    std::function<::Widgets::IWidget*(::Widgets::IWidget*, NbPoint<int>)> findDeepestWidget;
                    findDeepestWidget = [&](::Widgets::IWidget* current, NbPoint<int> p) -> ::Widgets::IWidget* {
                        const auto& subChildren = current->getChildrens();
                        // Идем с конца (rbegin), так как последние добавленные обычно сверху
                        for (auto it = subChildren.rbegin(); it != subChildren.rend(); ++it) {
                            auto* sub = it->get();
                            if (sub && !sub->isHide() && !sub->isDisable() && sub->hitTest(p)) {
                                // Рекурсия: ищем еще глубже
                                return findDeepestWidget(sub, p);
                            }
                        }
                        return current; // Глубже ничего нет, возвращаем текущий
                    };

                    // --- 2. ОСНОВНАЯ ЛОГИКА ОБРАБОТКИ ---

                    bool isFocusChanged = false;
                    ::Widgets::IWidget* clickedTarget = nullptr;

                    // Проход через дерево макета (DFS)
                    nbstl::dfs(
                        this->getLayoutRoot(),
                        getSortedChildren,
                        [&](const NNsLayout::LayoutNode* node)
                        {
                            if (auto widgetLayout = dynamic_cast<const NNsLayout::LayoutWidget*>(node))
                            {
                                auto rootWidget = widgetLayout->getWidget().get();
                                if (rootWidget && !rootWidget->isHide() && rootWidget->hitTest(point))
                                {
                                    clickedTarget = findDeepestWidget(rootWidget, point);

                                    if (clickedTarget) {
                                        isFocusChanged = true;

                                        if (focusedWidget && focusedWidget != clickedTarget) {
                                            focusedWidget->setUnfocused();
                                        }
                                        
                                        focusedWidget = clickedTarget;
                                        focusedWidget->setFocused();
                                        clicked = true;

                                        focusedWidget->onClick();
                                        
                                        return true; 
                                    }
                                }
                            }
                            return false;
                        }
                    );

                    // ВТОРОЙ ПРОХОД: Логика hitTestClick (нужна для спец-обработки, например ComboBox)
                    if (clickedTarget) {
                        nbstl::dfs(
                            this->getLayoutRoot(),
                            getSortedChildren,
                            [&](const NNsLayout::LayoutNode* node)
                            {
                                if (auto widgetLayout = dynamic_cast<const NNsLayout::LayoutWidget*>(node))
                                {
                                    auto widget = widgetLayout->getWidget().get();
                                    // Здесь используем hitTestClick, чтобы виджеты могли обработать клик 
                                    // на более глубоком логическом уровне (например, открытие списка)
                                    if (widget && !widget->isHide() && widget->hitTestClick(point))
                                    {
                                        return true; 
                                    }
                                }
                                return false;
                            }
                        );
                    }

                    // --- 3. ОБРАБОТКА КЛИКА "В ПУСТОТУ" ---
                    if (!isFocusChanged)
                    {
                        if (focusedWidget) {
                            focusedWidget->setUnfocused();
                            focusedWidget = nullptr;
                        }
                        // Закрываем глобальные элементы (например, выпадающие списки)
                        //::Widgets::ComboBox::closeAllDropDowns();
                    }

                    InvalidateRect(hWnd, nullptr, FALSE);
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
                NbPoint<int> point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

                MouseState mouseState = 
                {
                    .position = point
                };

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

                        if (auto widgetNode = dynamic_cast<NNsLayout::LayoutWidget*>(node);
                            widgetNode != nullptr)
                        {
                            if (auto widget = widgetNode->getWidget())
                            {
                                for (auto& w : widget->getChildrens())
                                {
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

                                if (!widget->isHoverStateDisable() && !isHoveredSet &&
                                    widget->hitTest(point))
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
                {
                    return 0;
                }

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

                return FALSE;
            }
            case WM_LBUTTONUP:
            {
                ReleaseCapture();
                dragging = false;

                NbPoint<int> point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

                if (auto pressedWidget = nbui::GlobalWidgetContext::getPressedWidget(); pressedWidget)
                {
                    pressedWidget->onRelease();
                    nbui::GlobalWidgetContext::releasePressedWidget();
                    return 0;
                }

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

                // Ищем самый верхний виджет под курсором
                nbstl::dfs(
                    this->getLayoutRoot(), getSortedChildren,
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

                InvalidateRect(hWnd, nullptr, FALSE);
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
            case WM_SHOWWINDOW:
            {
                if (static_cast<bool>(wParam) == true)
                {
                    EnableWindow(parent->getHandle().as<HWND>(), false);
                }
                else
                {
                    EnableWindow(parent->getHandle().as<HWND>(), TRUE);
                    SetForegroundWindow(parent->getHandle().as<HWND>());
                }
                return DefWindowProc(hWnd, message, wParam, lParam);
            }

            case WM_CLOSE:
            {
                HWND hParent = (parent) ? parent->getHandle().as<HWND>() : NULL;

                if (hParent)
                {
                    EnableWindow(hParent, TRUE);
                    SetForegroundWindow(hParent);
                }


                DestroyWindow(hWnd);
                return 0;
            }

            case WM_DESTROY:
            {
                focusedWidget = nullptr;

                onClose.emit();
                //onClose.disconnectAll();

                return 0;
            }
            case WM_NCDESTROY:
            {
                SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
                return 0;
            }


            }
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        
        inline static LRESULT CALLBACK staticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            ModalWindow *pThis = reinterpret_cast<ModalWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
            
            if (message == WM_NCCREATE)
            {
                auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
                pThis = static_cast<ModalWindow*>(cs->lpCreateParams);
                SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
            }

            if (pThis)
                return pThis->wndProc(hWnd, message, wParam, lParam);
            return DefWindowProc(hWnd, message, wParam, lParam);
        }



    };
};

#endif