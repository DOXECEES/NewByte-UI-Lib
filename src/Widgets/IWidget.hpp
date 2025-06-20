#ifndef SRC_WIDGETS_IWIDGET_HPP
#define SRC_WIDGETS_IWIDGET_HPP

#include "../Core.hpp"

#include <functional>

namespace Widgets
{
    class IWidget
    {
    public:
        
        IWidget(NbRect<int> rect) : rect(rect) {}
        virtual ~IWidget() = default;
        virtual void onClick() = 0;
        virtual bool hitTest(const NbPoint<int>& pos) = 0;
        
        const NbRect<int>& getRect() const { return rect; }
        const NbColor& getColor() const { return color; }


    protected:

        NbRect<int> rect = { 0, 0, 0, 0 };
        NbColor color = { 255, 255, 255 };

        std::function<void()> onClickCallback;

    };
}

#endif