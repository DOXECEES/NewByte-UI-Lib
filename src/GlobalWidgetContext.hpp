#ifndef NBUI_SRC_GLOBALWIDGETCONTEXT_HPP
#define NBUI_SRC_GLOBALWIDGETCONTEXT_HPP

#include "Core.hpp"

#include "MouseState.hpp"

namespace Widgets
{
    class IWidget;
}

namespace nbui
{
    class GlobalWidgetContext
    {
    public:
        static void captureWidget(Widgets::IWidget* widget) noexcept;
        static void releaseWidget(Widgets::IWidget* widget) noexcept;
        static Widgets::IWidget* getCapturedWidget() noexcept;

        static void onMouseMove(const MouseState& pos) noexcept;
        
        static void capturePressedWidget(Widgets::IWidget* widget) noexcept;
        static void releasePressedWidget() noexcept;
        static Widgets::IWidget* getPressedWidget() noexcept;

        static void              captureFocusedWidget(Widgets::IWidget* widget) noexcept;
        static void              releaseFocusedWidget() noexcept;
        static Widgets::IWidget* getFocusedWidget() noexcept;


        static void onUnfocus() noexcept;
        static void onSymbol(wchar_t symbol) noexcept;
        static void onPress() noexcept;
        static void onRelease() noexcept;


    private:
        inline static Widgets::IWidget* capturedWidget = nullptr;
        inline static Widgets::IWidget* pressedWidget = nullptr;
        inline static Widgets::IWidget* focusedWidget  = nullptr;

    };

};


#endif 