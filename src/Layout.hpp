#ifndef SRC_LAYOUT_HPP
#define SRC_LAYOUT_HPP

#include "Core.hpp"

#include "WindowInterface/IWindow.hpp"

#include <vector>

struct Padding
{
    int left    = 5;
    int top     = 5;
    int right   = 5;
    int bottom  = 5;
};

NbRect<int> applyPaddingToRect(const NbRect<int>& rect, const Padding& padding);


class Layout
{
public:
    Layout(WindowInterface::IWindow *window);
    virtual ~Layout() = default;

    void addWidget(Widgets::IWidget *widget);

    void setSpacing(const int spacing) noexcept;

protected:

    virtual void caclulateLayout() noexcept = 0;

    std::vector<WindowInterface::IWindow *> linkedWidgets;
    int spacing = 5;
};


class VBoxLayout : public Layout
{
public:

};


#endif