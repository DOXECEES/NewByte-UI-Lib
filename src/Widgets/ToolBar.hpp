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
            int width = 0;
            int height = 0;

            for (auto& child : childrens)
            {
                const NbSize<int>& size = child->measure(maxSize);

                width += size.width;
                height = std::max(height, size.height);
            }

            measuredSize = {width, height};
            return measuredSize;

        };


        void layout(const NbRect<int>& rect) noexcept override
        {
            int x = rect.x;

            for (auto& child : childrens)
            {
                const NbSize<int>& size = child->getMeasuredSize();

                child->layout({x, rect.y, size.width, rect.height});

                x += size.width;
            }
        
        };



    };

};


#endif