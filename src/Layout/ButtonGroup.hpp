#ifndef SRC_LAYOUT_BUTTONGROUP_HPP
#define SRC_LAYOUT_BUTTONGROUP_HPP

#include "Core.hpp"

#include "LayoutNode.hpp"
#include "LayoutWidget.hpp"

namespace NNsLayout
{
    enum class ButtonGroupType
    {
        ONLY_ONE,
        MULTI,
    };


    class ButtonGroup : public LayoutWidget
    {
    public:
        
        ButtonGroup(
            ButtonGroupType type = ButtonGroupType::ONLY_ONE,
            bool allowNone = false
        ) noexcept;
        
        void measure(const NbSize<int>& available) noexcept override;
        
        void layout(const NbRect<int>& bounds) noexcept override;
        
        void syncInternalState();
        
        void handleSelection(LayoutWidget* selectedNode);

        void setType(ButtonGroupType type) noexcept;

        void setCheckedIndex(bool state, int index) noexcept;

    private:

        ButtonGroupType type = ButtonGroupType::ONLY_ONE;

        bool allowNoneSelected = false;
    };
};


#endif