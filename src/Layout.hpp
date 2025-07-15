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

        std::vector<NbRect<int>> layoutRects;
        layoutRects.reserve(linkedLayouts.size() + 1);

        if(!linkedLayouts.empty())
        {
            int dxLayout = clientRect.width;
            int dylayout = clientRect.height / (linkedLayouts.size() + 1);  // because parent layout is also counted

            for(int i = 0; i < linkedLayouts.size() + 1; i++)
            {
                NbRect<int> rect = {
                    clientRect.x,
                    clientRect.y + (i) * dylayout,
                    dxLayout,
                    dylayout
                };

                layoutRects.push_back(rect);
                if(i != 0)
                    linkedLayouts[i - 1]->window->setClientRect(rect);
            }
        }

        //calculate parent layout widgets

        if(!linkedWidgets.empty())
        {
            int countOfWidgets = linkedWidgets.size();

            int dx = layoutRects[0].width / countOfWidgets;
            int dy = layoutRects[0].height / countOfWidgets;

            int currentIteration = 0;
            for(auto& widget : linkedWidgets)
            {
                NbRect<int> rect = {
                    layoutRects[0].x ,
                    layoutRects[0].y + currentIteration * dy,
                    layoutRects[0].width,
                    dy
                };

                rect = applyPaddingToRect(rect, Padding());

                widget->setRect(rect);
                currentIteration++;
            }
        }

        // other layouts widgets
        for (int i = 1; i < linkedLayouts.size() + 1; i++)
        {
            if(linkedWidgets.empty()) continue;
            int countOfWidgets = linkedWidgets.size();

            int dx = layoutRects[i].width / countOfWidgets;
            int dy = layoutRects[i].height / countOfWidgets;

            int currentIteration = 0;
            for(auto& widget : linkedWidgets)
            {
                NbRect<int> rect = {
                    layoutRects[i].x ,
                    layoutRects[i].y + currentIteration * dy,
                    layoutRects[i].width,
                    dy
                };

                rect = applyPaddingToRect(rect, Padding());

                widget->setRect(rect);
                currentIteration++;
            }
        }

        // int countOfWidgets = linkedWidgets.size();

        // int dx = clientRect.width / countOfWidgets;
        // int dy = clientRect.height / countOfWidgets;

        // int currentIteration = 0;
        // for(auto& widget : linkedWidgets)
        // {
        //     NbRect<int> rect = {
        //         clientRect.x ,
        //         clientRect.y + currentIteration * dy,
        //         clientRect.width,
        //         dy
        //     };

        //     rect = applyPaddingToRect(rect, Padding());

        //     widget->setRect(rect);
        //     currentIteration++;
        // }
    }

};


#endif 