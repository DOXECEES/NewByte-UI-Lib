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
            dirty = true;
            return children.back().get();
        }

        void clearChilds() noexcept
        {
            children.clear();
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


    class GridLayout : public LayoutNode
    {
    public:
        GridLayout(int cols) noexcept
            : LayoutNode(nullptr),
              columns(cols)
        {
        }

        void measure(const NbSize<int>& available) noexcept override;
        void layout(const NbRect<int>& bounds) noexcept override;

        int columns = 1;
        int rows = 0; // 0 = авто
        // Списки пропорций для колонок/строк (как в WPF или Unity)
        std::vector<float> columnWeights;
        int spacing = 0; // Расстояние между ячейками

    };

    class FlowLayout : public LayoutNode
    {
    public:
        FlowLayout() noexcept
            : LayoutNode(nullptr)
        {
        }
        // Расчет необходимых размеров
        void measure(const NbSize<int>& available) noexcept override
        {
            if (children.empty())
            {
                measuredSize = {0, 0};
                return;
            }

            // 1. Предполагаем, что все карточки одинаковые. Берем размер первой.
            auto& firstStyle = children[0]->style;

            // Ширина и высота одного элемента с учетом его внешних отступов (Margin)
            int itemW = static_cast<int>(firstStyle.width) + firstStyle.margin.left +
                        firstStyle.margin.right;
            int itemH = static_cast<int>(firstStyle.height) + firstStyle.margin.top +
                        firstStyle.margin.bottom;

            // 2. Считаем, сколько колонок влезет в доступную ширину
            int columns = available.width / itemW;
            if (columns < 1)
            {
                columns = 1;
            }

            // 3. Считаем количество необходимых строк
            int totalItems = static_cast<int>(children.size());
            int rows = (totalItems + columns - 1) / columns;

            // Итоговый размер: ширина — на всё доступное место, высота — сколько заняли строки
            measuredSize.width = available.width;
            measuredSize.height = rows * itemH;

            // Ограничения Min/Max
            measuredSize.height = (std::max)(measuredSize.height, style.minSize.height);
            if (style.maxSize.height > 0)
            {
                measuredSize.height = (std::min)(measuredSize.height, style.maxSize.height);
            }
        }

        // Расстановка элементов
        void layout(const NbRect<int>& bounds) noexcept override
        {
            layoutRect = bounds;
            if (children.empty())
            {
                return;
            }

            // Берем параметры базового элемента
            auto& firstStyle = children[0]->style;
            int childW = static_cast<int>(firstStyle.width);
            int childH = static_cast<int>(firstStyle.height);

            // Полный шаг по сетке (контент + отступы)
            int stepX = childW + firstStyle.margin.left + firstStyle.margin.right;
            int stepY = childH + firstStyle.margin.top + firstStyle.margin.bottom;

            // Вычисляем количество колонок
            int columns = bounds.width / stepX;
            if (columns < 1)
            {
                columns = 1;
            }

            // Центрирование сетки (опционально):
            // Считаем остаток места справа и делим его на 2, чтобы сетка была по центру
            int totalGridWidth = columns * stepX;
            int offsetX = (bounds.width - totalGridWidth) / 2;

            int index = 0;
            for (auto& child : children)
            {
                int col = index % columns;
                int row = index / columns;

                auto& st = child->style;

                // Позиция:
                // X = Лево + ОфсетЦентровки + (номер_колонки * шаг) + Левый_Margin
                int x = bounds.x + offsetX + (col * stepX) + st.margin.left;

                // Y = Верх + (номер_строки * шаг) + Верхний_Margin
                int y = bounds.y + (row * stepY) + st.margin.top;

                // Устанавливаем Rect для ребенка
                NbRect<int> childRect{x, y, childW, childH};

                child->setRect(childRect);
                child->layout(childRect);

                index++;
            }
        }
    };


    class LayoutWidget : public LayoutNode
    {
    public:
        explicit LayoutWidget(Widgets::IWidget* w) noexcept;
       

        void setWidget(std::shared_ptr<Widgets::IWidget> w) noexcept;
       

        std::shared_ptr<Widgets::IWidget> getWidget() const noexcept
        {
            return widget;
        }


        void measure(const NbSize<int>& available) noexcept override;
        void layout(const NbRect<int>& bounds) noexcept override;

    private:
        std::shared_ptr<Widgets::IWidget> widget;
    };


}

#endif
