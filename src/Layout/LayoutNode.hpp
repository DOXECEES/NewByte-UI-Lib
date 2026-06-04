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
        NbColor         color           ;
        NbSize<int>     minSize         ;
        NbSize<int>     maxSize         ;

        SizeType        widthSizeType   ;
        SizeType        heightSizeType  ;

        float           width           ;
        float           height          ;

        Border          border          ;

        Margin<float>   margin          ;
        Padding<float>  padding         ;
    };

    class LayoutNode
    {
    public:
        explicit LayoutNode(Widgets::IWidget* owner) noexcept;

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
            dirty = true;
            return children.back().get();
        }

        void clearChilds() noexcept;

        void markDirty() noexcept;

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

        void setSpacing(int value) noexcept;

    private:
        int scrollOffset; 
        int spacing = 0;
    };


    class GridLayout : public LayoutNode
    {
    public:
        GridLayout(int cols) noexcept;

        void measure(const NbSize<int>& available) noexcept override;
        void layout(const NbRect<int>& bounds) noexcept override;

        int columns = 1;
        int rows = 0; 
        std::vector<float> columnWeights;
        int spacing = 0; 

    };

    class FlowLayout : public LayoutNode
    {
    public:
        FlowLayout() noexcept;
        void measure(const NbSize<int>& available) noexcept override;
    
        void layout(const NbRect<int>& bounds) noexcept override;
        
    };


}

#endif
