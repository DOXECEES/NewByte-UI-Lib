#ifndef NBUI_SRC_WIDGETS_TREEVIEW_HPP
#define NBUI_SRC_WIDGETS_TREEVIEW_HPP

#include "IWidget.hpp"
#include "Signal.hpp"

#include "WidgetStyle.hpp"
#include "Theme.hpp"

#include <Uuid.hpp>

#include <vector>
#include <memory>
#include <functional>
#include <optional>
#include <unordered_map>
#include <string>
#include <Alghorithm.hpp>

namespace Widgets
{
    class ModelItem;

    class ModelIndex
    {
    public:
        ModelIndex() noexcept = default;
        explicit ModelIndex(const nbstl::Uuid& id) noexcept : id(id) {}
        ~ModelIndex() = default;

        bool isValid() const noexcept { return !(id == nbstl::Uuid{}); }
        const nbstl::Uuid& getUuid() const noexcept { return id; }

        bool operator==(const ModelIndex& o) const noexcept { return id == o.id; }
        bool operator!=(const ModelIndex& o) const noexcept { return !(*this == o); }

    private:
        nbstl::Uuid id;
    };

    class ITreeModel
    {
    public:
        virtual ~ITreeModel() = default;

        virtual const std::vector<std::unique_ptr<ModelItem>>& getRootItems() const noexcept = 0;

        virtual const ModelItem* findById(const nbstl::Uuid& id) const noexcept = 0;

        virtual std::string data(const ModelItem& item) const noexcept = 0;

        virtual void forEach(std::function<void(const ModelItem&)> func) const noexcept = 0;

        virtual size_t size() const noexcept = 0;
    };


    class ModelItem
    {
    public:
        ModelItem() = default;
        ModelItem(void* userData, ModelItem* parent, size_t depth) noexcept
            : data(userData), parent(parent), depth(depth) {}

        ~ModelItem() = default;

        ModelItem(const ModelItem&) = delete;
        ModelItem& operator=(const ModelItem&) = delete;

        ModelItem(ModelItem&&) noexcept = default;
        ModelItem& operator=(ModelItem&&) noexcept = default;

        const nbstl::Uuid& getUuid() const noexcept { return uuid; }
        size_t getDepth() const noexcept { return depth; }
        void* getData() const noexcept { return data; }

        bool haveChildrens() const noexcept;

        nbstl::Uuid uuid = nbstl::Uuid::generate();
        void* data = nullptr;
        ModelItem* parent = nullptr;
        std::vector<std::unique_ptr<ModelItem>> children;
    
    private:
        size_t depth = 0;
    };


    class TreeView : public IWidget
    {
    public:
        static constexpr const char* CLASS_NAME = "TreeView";
        static constexpr size_t HEIGHT_OF_ITEM_IN_PIXEL = 20;

        enum class ItemState : uint8_t
        {
            COLLAPSED = 0,
            EXPANDED = 1,
            SELECTED = 2,
        };

        explicit TreeView(const NbRect<int>& rect) noexcept;
        ~TreeView() override;

        bool hitTest(const NbPoint<int>& pos) override;
        bool hitTestClick(const NbPoint<int>& pos) noexcept override;
        const char* getClassName() const override;
        NB_NODISCARD const TreeViewStyle& getTreeViewStyle() const noexcept;

        void setModel(const std::shared_ptr<ITreeModel>& modelParam) noexcept;
        std::shared_ptr<ITreeModel> getModel() const noexcept { return model; }

        ModelIndex indexFromVisibleRow(size_t row) const noexcept; 
        std::optional<size_t> visibleRowFromIndex(const ModelIndex& index) const noexcept; 

        const ModelItem& getItemByIndex(const ModelIndex& index) const noexcept;

        ItemState getItemState(const ModelItem& item) const noexcept;
        void setItemExpanded(const ModelIndex& index, bool expanded) noexcept;
        bool isItemExpanded(const ModelIndex& index) const noexcept;
        
        void setItemState(const ModelIndex& index, ItemState state) noexcept;
        void rebuildVisibleList() noexcept;

        const ModelItem* getVisibleItem(size_t index) const noexcept;
        bool isItemSelected(const ModelIndex& index) const noexcept;
        bool isItemHaveChildrens(const ModelIndex& index) const noexcept;

        size_t getVisibleCount() const noexcept;
        size_t hitElement(const NbPoint<int>& pos) const noexcept;
        size_t getMaxCountOfItems() const noexcept;
        ModelIndex getLastClickIndex() const noexcept;
        ModelIndex getLastHitIndex() const noexcept;

        virtual const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
        {
            if (!model)
            {
                measuredSize = { 0, 0 };
                return measuredSize;
            }

            const size_t visibleCount = getVisibleCount();

            const int contentHeight =
                static_cast<int>(visibleCount * HEIGHT_OF_ITEM_IN_PIXEL);

            measuredSize.width = maxSize.width;
            measuredSize.height = nbstl::min(contentHeight, maxSize.height);

            return measuredSize;
        }

        virtual void layout(const NbRect<int>& rect) noexcept override
        {
            this->rect= rect;

            const int itemHeight = static_cast<int>(HEIGHT_OF_ITEM_IN_PIXEL);

            const size_t totalVisible = getVisibleCount();

            if (totalVisible == 0)
            {
                range = { 0, 0 };
                return;
            }

            const size_t firstVisible =
                static_cast<size_t>(nbstl::max(0, rect.y / itemHeight));

            const size_t visibleItemsCount =
                static_cast<size_t>((rect.height + itemHeight - 1) / itemHeight);

            const size_t lastVisible =
                nbstl::min(firstVisible + visibleItemsCount, totalVisible);

            range.first = firstVisible;
            range.second = lastVisible;

        }


    public: 
        Signal<void(const ModelIndex&)> onItemClickSignal;
        Signal<void(const ModelIndex&)> onItemButtonClickSignal;
        Signal<void(const ModelIndex&)> onItemChangeSignal;

    private:
        NbRect<int> geometry;
        NbSize<int> measuredSize;



        TreeViewStyle                                       treeViewStyle = ThemeManager::getCurrent().treeViewStyle;

        struct NodeState
        {
            bool expanded = false;
            bool selected = false;
        };

        std::shared_ptr<ITreeModel>                         model               = nullptr;
        std::unordered_map<nbstl::Uuid, const ModelItem*>   uuidMap;
        std::unordered_map<nbstl::Uuid, NodeState>          nodeStates;
        std::vector<const ModelItem*>                       visibleItems; 

        ModelIndex                                          lastHitIndex;
        ModelIndex                                          lastClickedIndex;

        std::pair<size_t, size_t>                           range               = { 0 , 0 };

        void buildUuidMap() noexcept;
        void collectVisibleRecursive(const ModelItem* node) noexcept;
    };

};

#endif 
