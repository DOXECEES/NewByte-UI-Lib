#include "LayoutWindow.hpp"

#include "WindowInterface/IWindow.hpp"

namespace NNsLayout
{
    LayoutWindow::LayoutWindow(WindowInterface::IWindow* w) noexcept
        : LayoutNode(nullptr),
          ownerWindow(w)
    {
    }
    void LayoutWindow::setOwnerWindow(WindowInterface::IWindow* w) noexcept
    {
        ownerWindow = w;
        dirty = true;
    }
    WindowInterface::IWindow* LayoutWindow::getOwnerWindow() const noexcept
    {
        return ownerWindow;
    }
    void LayoutWindow::measure(const NbSize<int>& available) noexcept
    {
        if (!children.empty())
        {
            children[0]->measure(available);
            measuredSize = children[0]->getMeasuredSize();
        }
        else
        {
            measuredSize = available;
        }
    }

    void LayoutWindow::layout(const NbRect<int>& bounds) noexcept
    {
        if (!ownerWindow)
        {
            return;
        }

        if (!children.empty())
        {
            NbRect<int> client = ownerWindow->getClientRect();

            children[0]->layout(client);
        }
    }

};