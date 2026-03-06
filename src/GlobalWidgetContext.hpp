#ifndef NBUI_SRC_GLOBALWIDGETCONTEXT_HPP
#define NBUI_SRC_GLOBALWIDGETCONTEXT_HPP

#include "Core.hpp"

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

        static void onMouseMove(const NbPoint<int>& pos) noexcept;

        static void capturePressedWidget(Widgets::IWidget* widget) noexcept;
        static void releasePressedWidget() noexcept;
        static Widgets::IWidget* getPressedWidget() noexcept;

        static void onPress() noexcept;
        static void onRelease() noexcept;


    private:
        inline static Widgets::IWidget* capturedWidget = nullptr;
        inline static Widgets::IWidget* pressedWidget = nullptr;
    };

};


#endif 