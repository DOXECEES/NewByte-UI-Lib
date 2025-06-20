#ifndef SRC_WIDGETS_BUTTON_HPP
#define SRC_WIDGETS_BUTTON_HPP

#include "IWidget.hpp"

namespace Widgets
{
    class Button : public IWidget
    {
    public:
        Button(NbRect<int> rect) : IWidget(rect) {};
        
        void onClick() override;
        bool hitTest(const NbPoint<int>& pos) override;
    };
}

#endif