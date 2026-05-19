#include "GlobalWidgetContext.hpp"

#include "Widgets/IWidget.hpp"

namespace nbui
{
    void GlobalWidgetContext::captureWidget(Widgets::IWidget* widget) noexcept
    {
        capturedWidget = widget;
    }

    void GlobalWidgetContext::releaseWidget(Widgets::IWidget* widget) noexcept
    {
        capturedWidget = nullptr;
    }

    Widgets::IWidget* GlobalWidgetContext::getCapturedWidget() noexcept
    {
        return capturedWidget;
    }


    void GlobalWidgetContext::onMouseMove(const MouseState& pos) noexcept
    {
        if (!capturedWidget)
        {
            return;
        }

        capturedWidget->onMouseMove(pos);
    }

    void GlobalWidgetContext::capturePressedWidget(Widgets::IWidget* widget) noexcept
    {
        pressedWidget = widget;
    }

    void GlobalWidgetContext::releasePressedWidget() noexcept
    {
        pressedWidget = nullptr;
    }

    Widgets::IWidget* GlobalWidgetContext::getPressedWidget() noexcept
    {
        return pressedWidget;
    }

    void GlobalWidgetContext::captureFocusedWidget(Widgets::IWidget* widget) noexcept
    {
        focusedWidget = widget;
    }

    void GlobalWidgetContext::releaseFocusedWidget() noexcept
    {
        focusedWidget = nullptr;
    }

    Widgets::IWidget* GlobalWidgetContext::getFocusedWidget() noexcept
    {
        return focusedWidget;
    }

    void GlobalWidgetContext::onUnfocus() noexcept
    {
        if (focusedWidget)
        {
            focusedWidget->onUnfocus();
        }

    }

    void GlobalWidgetContext::onSymbol(wchar_t symbol) noexcept
    {
        focusedWidget->onSymbolButtonClicked(symbol);
    }

    void GlobalWidgetContext::onPress() noexcept
    {
        pressedWidget->onClick();
    }

    void GlobalWidgetContext::onRelease() noexcept
    {
        pressedWidget->onRelease();
    }




};


