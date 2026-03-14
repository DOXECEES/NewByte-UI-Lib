#include "ToolBar.hpp"

namespace Widgets
{
    bool ToolBar::hitTest(const NbPoint<int>& pos)
    {
        return rect.isInside(pos);
    }

    const char* ToolBar::getClassName() const
    {
        return CLASS_NAME;
    }

    void ToolBar::addToolBarWidget(
        std::shared_ptr<IWidget> widget,
        int size
    ) noexcept
    {
        widget->setSize({size, 35});
        childrens.push_back(widget);
    }

};

