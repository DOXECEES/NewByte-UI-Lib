#ifndef NBUI_SRC_LAYOUT_HPP
#define NBUI_SRC_LAYOUT_HPP

#include "Core.hpp"

#include "WindowInterface/IWindow.hpp"
#include "Widgets/IWidget.hpp"

#include <vector>

struct Padding
{
    int left    = 5;
    int top     = 5;
    int right   = 5;
    int bottom  = 5;
};

NbRect<int> applyPaddingToRect(const NbRect<int>& rect, const Padding& padding)
{
    return NbRect<int>(rect.x + padding.left, rect.y + padding.top, rect.width - padding.left - padding.right, rect.height - padding.top - padding.bottom);
}


class Layout : WindowInterface::IWindowStateChangedListener
{
public:
    Layout(WindowInterface::IWindow *window)
        :window(window)
    {
        window->addStateChangedListener(this);
    }

    virtual ~Layout() = default;

    void onSizeChanged(const NbSize<int>& size) override
    {
        caclulateLayout();
    }

    void addWidget(Widgets::IWidget *widget)
    {
        window->linkWidget(widget);
        linkedWidgets.push_back(widget);
    }

    void addLayout(Layout *layout)
    {
        linkedLayouts.push_back(layout);
    }

    void setSpacing(const int spacing) noexcept
    {
        this->spacing = spacing;
    }

    WindowInterface::IWindow*       window          = nullptr;
protected:

    virtual void caclulateLayout() noexcept = 0;

    std::vector<Widgets::IWidget*>  linkedWidgets;
    std::vector<Layout*>            linkedLayouts;
    int                             spacing         = 5;


};


class VBoxLayout : public Layout
{
public:
    VBoxLayout(WindowInterface::IWindow *window) : Layout(window) {}

    void caclulateLayout() noexcept override 
    {

        const NbRect<int>& clientRect = window->getClientRect();

        int countOfWidgets = linkedWidgets.size();
        int dx = clientRect.width / countOfWidgets;
        int dy = clientRect.height / countOfWidgets;

        int currentIteration = 0;
        for(auto& widget : linkedWidgets)
        {
            NbRect<int> rect = {
                0,
                currentIteration * dy,
                clientRect.width,
                dy
            };
            rect = applyPaddingToRect(rect, Padding());
            widget->setRect(rect);
            currentIteration++;
        }
    }

};


#endif 