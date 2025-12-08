#ifndef SRC_LAYOUT_LAYOUTNODE_HPP
#define SRC_LAYOUT_LAYOUTNODE_HPP

#include <vector>
#include <memory>

#include "Core.hpp"

#include "Widgets/Indentations.hpp"

#include "Widgets/WidgetStyle.hpp"

#undef ABSOLUTE
#undef RELATIVE

namespace Widgets { class IWidget; }
namespace WindowInterface { class IWindow; }

namespace NNsLayout
{
    enum class SizeType
    {
        ABSOLUTE,
        RELATIVE,
        FLEX,
        AUTO,
    };


    struct LayoutStyle
    {
        NbColor         color;
        NbSize<int>     minSize;
        NbSize<int>     maxSize;

        SizeType        widthSizeType;
        SizeType        heightSizeType;

        float           width;
        float           height;

        Border          border;

        Margin<float>   margin;
        Padding<float>  padding;
    };

    class LayoutNode
    {
    public:
        explicit LayoutNode(Widgets::IWidget* owner) noexcept
            : ownerWidget(owner)
        {
        }

        virtual ~LayoutNode() = default;

        LayoutNode(const LayoutNode&) = delete;
        LayoutNode& operator=(const LayoutNode&) = delete;

        LayoutNode(LayoutNode&&) noexcept = default;
        LayoutNode& operator=(LayoutNode&&) noexcept = default;

        template<typename T>
        LayoutNode* addChild(std::unique_ptr<T> child)
        {
            static_assert(std::is_base_of<LayoutNode, T>::value, "T must derive from LayoutNode");
            child->parent = this;
            children.emplace_back(std::move(child));
            return children.back().get();
        }


        void markDirty() noexcept
        {
            if (dirty) return;
            dirty = true;
            if (parent) parent->markDirty();
        }

        bool isDirty() const noexcept { return dirty; }

        inline LayoutNode* getParent() const noexcept { return parent; }
        inline Widgets::IWidget* getOwner() const noexcept { return ownerWidget; }
        const NbSize<int>& getMeasuredSize() const noexcept { return measuredSize; }
        const std::vector<std::unique_ptr<LayoutNode>>& getChildren() const noexcept { return children; }
        const LayoutNode* getChildrenAt(size_t index) const noexcept { return children.at(index).get(); }
        size_t getChildrenSize() const noexcept { return children.size(); }
        const NbRect<int>& getRect() const noexcept { return layoutRect; }
        void setRect(const NbRect<int>& rc) noexcept { layoutRect = rc; }
        virtual void measure(const NbSize<int>& available) noexcept = 0;
        virtual void layout(const NbRect<int>& bounds) noexcept = 0;

        LayoutStyle style;
    protected:
        std::vector<std::unique_ptr<LayoutNode>> children;
        LayoutNode* parent = nullptr;
        Widgets::IWidget* ownerWidget = nullptr;

        bool dirty = true;
        NbSize<int> measuredSize;
        NbRect<int> layoutRect;
    };


    class LayoutWindow : public LayoutNode
    {
    public:
        LayoutWindow(WindowInterface::IWindow* w) noexcept
            : LayoutNode(nullptr)
            , ownerWindow(w)
        {

        }

        void setOwnerWindow(WindowInterface::IWindow* w) noexcept {
            ownerWindow = w;
            dirty = true;
        }

        WindowInterface::IWindow* getOwnerWindow() const noexcept {
            return ownerWindow;
        }

        void measure(const NbSize<int>& available) noexcept override {
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

        void layout(const NbRect<int>& bounds) noexcept override;

    private:
        WindowInterface::IWindow* ownerWindow = nullptr;
    };


    class HLayout : public LayoutNode
    {
    public:
        HLayout() noexcept : LayoutNode(nullptr) {}

        void measure(const NbSize<int>& available) noexcept override;
        void layout(const NbRect<int>& bounds) noexcept override;
    };

    class VLayout : public LayoutNode
    {
    public:
        VLayout() noexcept : LayoutNode(nullptr), scrollOffset(0) {}

        void measure(const NbSize<int>& available) noexcept override;
        void layout(const NbRect<int>& bounds) noexcept override;

        void setScrollOffset(int offset) noexcept { scrollOffset = offset; }
        int getScrollOffset() const noexcept { return scrollOffset; }

    private:
        int scrollOffset; 
    };


    class LayoutWidget : public LayoutNode
    {
    public:
        explicit LayoutWidget(Widgets::IWidget* w) noexcept
            : LayoutNode(w), widget(w)
        {
        }

        void setWidget(std::shared_ptr<Widgets::IWidget> w) noexcept {
            widget = w;
            dirty = true;
        }

        std::shared_ptr<Widgets::IWidget> getWidget() const noexcept {
            return widget;
        }


        void measure(const NbSize<int>& available) noexcept override;
        void layout(const NbRect<int>& bounds) noexcept override;

    private:
        std::shared_ptr<Widgets::IWidget> widget;
    };


}

#endif
