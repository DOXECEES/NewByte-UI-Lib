#ifndef SRC_LAYOUT_LAYOUTWIDGET_HPP
#define SRC_LAYOUT_LAYOUTWIDGET_HPP

#include "LayoutNode.hpp"


namespace NNsLayout
{
    class LayoutWidget : public LayoutNode
    {
    public:
        explicit LayoutWidget(Widgets::IWidget* w) noexcept;
        explicit LayoutWidget(std::shared_ptr<Widgets::IWidget> w) noexcept;

        void setWidget(std::shared_ptr<Widgets::IWidget> w) noexcept;
       
        std::shared_ptr<Widgets::IWidget> getWidget() const noexcept;

        void measure(const NbSize<int>& available) noexcept override;
        void layout(const NbRect<int>& bounds) noexcept override;

    private:
        std::shared_ptr<Widgets::IWidget> widget;
    };


}

#endif
