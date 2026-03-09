#ifndef NBUI_SRC_WIDGETS_TOOLBAR_HPP
#define NBUI_SRC_WIDGETS_TOOLBAR_HPP

#include "IWidget.hpp"

namespace Widgets
{
    class ToolBar : public Widgets::IWidget
    {
    public:
        
        ToolBar() noexcept 
            : IWidget({})
        {

        }

        constexpr static const char* CLASS_NAME = "ToolBar";

        bool hitTest(const NbPoint<int>& pos) override;
        const char* getClassName() const override;

        void addToolBarWidget(std::shared_ptr<IWidget> widget, int size) noexcept;


        const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
        {
            return measuredSize;
        }
        void layout(const NbRect<int>& rect) noexcept override
        {
            this->rect = rect;
        }





    };

};


#endif