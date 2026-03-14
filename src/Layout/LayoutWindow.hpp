#ifndef SRC_LAYOUT_LAYOUTWINDOW_HPP
#define SRC_LAYOUT_LAYOUTWINDOW_HPP

#include "LayoutNode.hpp"

namespace Widgets { class IWidget; }
namespace WindowInterface { class IWindow; }

namespace NNsLayout 
{
class LayoutWindow : public LayoutNode
    {
    public:
        LayoutWindow(WindowInterface::IWindow* w) noexcept;

        void setOwnerWindow(WindowInterface::IWindow* w) noexcept;

        WindowInterface::IWindow* getOwnerWindow() const noexcept;

        void measure(const NbSize<int>& available) noexcept override;

        void layout(const NbRect<int>& bounds) noexcept override;

    private:
        WindowInterface::IWindow* ownerWindow = nullptr;
    };

};

#endif