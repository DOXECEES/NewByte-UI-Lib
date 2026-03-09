#include "ButtonGroup.hpp"
// #include "ButtonGroup.hpp"
//
//namespace Widgets
//{
//    bool ButtonGroup::hitTest(const NbPoint<int>& pos)
//    {
//        for (auto& i : childrens)
//        {
//            if (i->hitTest(pos))
//            {
//                i->setDisable();
//            }
//            else
//            {
//                i->setActive();
//            }
//        }
//        return rect.isInside(pos);
//    }
//
//    bool ButtonGroup::hitTestClick(const NbPoint<int>& pos) noexcept
//    {
//        for (auto& i : childrens)
//        {
//            if (i->hitTest(pos))
//            {
//                i->setDisable();
//            }
//            else
//            {
//                i->setActive();
//            }
//        }
//        return rect.isInside(pos);
//    }
//
//    const char* ButtonGroup::getClassName() const
//    {
//        return CLASS_NAME;
//    }
//
//};
//

bool ButtonGroupWidget::hitTest(const NbPoint<int>& pos)
{
    return rect.isInside(pos);
}

const char* ButtonGroupWidget::getClassName() const
{
    return CLASS_NAME;
}
