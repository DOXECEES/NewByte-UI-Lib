#ifndef NBUI_SRC_WIDGETS_BUTTONGROUP_HPP
#define NBUI_SRC_WIDGETS_BUTTONGROUP_HPP

#include "IWidget.hpp"

class ButtonGroupWidget : public Widgets::IWidget
{
public:
    DECLARE_WIDGET_CLASS_NAME(ButtonGroupWidget);
    ButtonGroupWidget() 
        : IWidget({})
    {

    }
    // Inherited via IWidget
    bool hitTest(const NbPoint<int>& pos) override;
    const char* getClassName() const override;
    // Пустой виджет-контейнер
};


#endif