#include "TreeView.hpp"
#include "Debug.hpp" 

#include <algorithm>
#include <stack>

namespace Widgets
{

    // SetText - ID
    // locales 
    // ru 
    // eng - default 
    //
    TreeView::TreeView(const NbRect<int>& rect) noexcept
        : IWidget(rect)
    {
        range.second = rect.height;

		subscribe(this, &Widgets::TreeView::onItemButtonClickSignal, [this](Widgets::ModelIndex index) {
            if (!index.isValid())
            {
				return;
            }

			auto model = this->getModel();
            if (!model)
            {
                return;
            }

			auto itemOpt = model->findById(index.getUuid());
            if (!itemOpt)
            {
                return;
            }

			const auto& item = itemOpt;

			auto currentState = this->getItemState(*item);
			auto newState = (currentState == Widgets::TreeView::ItemState::EXPANDED)
				? Widgets::TreeView::ItemState::COLLAPSED
				: Widgets::TreeView::ItemState::EXPANDED;
			this->setItemState(index, newState);
	    });
    }

    TreeView::~TreeView()
    {
        // TODO: unsubscribe
    }

    bool TreeView::hitTest(const NbPoint<int>& pos)
    {
        size_t index = hitElement(pos);
        lastHitIndex = indexFromVisibleRow(index);
        return true;
    }

    bool TreeView::hitTestClick(const NbPoint<int>& pos) noexcept
    {
        if (!model)
        {
            return false;
        }

        size_t row = hitElement(pos);
        static ModelIndex prevClickedIndex;
        lastClickedIndex = indexFromVisibleRow(row);
        if (!lastClickedIndex.isValid())
        {
            return false;
        }
   
        const ModelItem* item = uuidMap.at(lastClickedIndex.getUuid());
        

        if (isItemHaveChildrens(lastClickedIndex))
        {
            NbRect<int> buttonRect = {
                (int)20 * (int)item->getDepth(),
                (int)row * 20,
                20,
                20
            };
            if (buttonRect.isInside(pos))
            {
                onItemButtonClickSignal.emit(lastClickedIndex);
            }
            else
            {
                onItemClickSignal.emit(lastClickedIndex);
                if (prevClickedIndex != lastClickedIndex)
                {
                    onItemChangeSignal.emit(lastClickedIndex);
                }
            }

        }
        else
        {
            onItemClickSignal.emit(lastClickedIndex);
            if (prevClickedIndex != lastClickedIndex)
            {
                onItemChangeSignal.emit(lastClickedIndex);
            }
        }
        prevClickedIndex = lastClickedIndex;
        return true;
    }

    const char* TreeView::getClassName() const
    {
        return CLASS_NAME;
    }

    const TreeViewStyle& TreeView::getTreeViewStyle() const noexcept
    {
        return treeViewStyle;
    }

    void TreeView::setModel(const std::shared_ptr<ITreeModel>& modelParam) noexcept
    {
        model = modelParam;
        uuidMap.clear();
        nodeStates.clear();
        visibleItems.clear();

        if (!model) return;

        buildUuidMap();

        for (auto& kv : uuidMap)
        {
            nodeStates.emplace(kv.first, NodeState{ false, false });
        }

        for (const auto& rootPtr : model->getRootItems())
        {
            nodeStates[rootPtr->getUuid()].expanded = true;
        }

        rebuildVisibleList();
    }

    void TreeView::buildUuidMap() noexcept
    {
        if (!model) return;
        model->forEach([this](const ModelItem& item)
        {
            uuidMap.emplace(item.getUuid(), &item);
        });
    }

    void TreeView::rebuildVisibleList() noexcept
    {
        visibleItems.clear();
        if (!model) return;

        for (const auto& root : model->getRootItems())
        {
            collectVisibleRecursive(root.get());
        }
    }

    const ModelItem* TreeView::getVisibleItem(size_t index) const noexcept
    {
        return (index < visibleItems.size()) ? visibleItems[index] : nullptr;
    }

    bool TreeView::isItemSelected(const ModelIndex& index) const noexcept
    {
        auto it = nodeStates.find(index.getUuid());
        return it != nodeStates.end() && it->second.selected;
    }

    bool TreeView::isItemHaveChildrens(const ModelIndex& index) const noexcept
    {
        if (!index.isValid())
        {
            return false;
        }
        const nbstl::Uuid& uuid = index.getUuid();
        return uuidMap.at(uuid)->haveChildrens();
    }

    void TreeView::collectVisibleRecursive(const ModelItem* node) noexcept
    {
        if (!node) return;
        visibleItems.push_back(node);

        const auto it = nodeStates.find(node->getUuid());
        const bool expanded = (it != nodeStates.end()) ? it->second.expanded : false;

        if (expanded)
        {
            for (const auto& child : node->children)
                collectVisibleRecursive(child.get());
        }
    }

    ModelIndex TreeView::indexFromVisibleRow(size_t row) const noexcept
    {
        if (row >= visibleItems.size())
        {
            return ModelIndex{};
        }
        return ModelIndex(visibleItems[row]->getUuid());
    }

    std::optional<size_t> TreeView::visibleRowFromIndex(const ModelIndex& index) const noexcept
    {
        if (!index.isValid()) return std::nullopt;
        const nbstl::Uuid& id = index.getUuid();
        for (size_t i = 0; i < visibleItems.size(); ++i)
            if (visibleItems[i]->getUuid() == id)
                return i;
        return std::nullopt;
    }

    const ModelItem& TreeView::getItemByIndex(const ModelIndex& index) const noexcept
    {
        const nbstl::Uuid& uuid = index.getUuid();
        if (uuidMap.find(uuid) != uuidMap.cend())
        {
            NB_ASSERT(true, "no such index in uuidMap");
        }
        
        return *uuidMap.at(uuid);
    }

    size_t TreeView::hitElement(const NbPoint<int>& pos) const noexcept
    {
        size_t elementIndex = range.first + pos.y;
        return elementIndex / HEIGHT_OF_ITEM_IN_PIXEL;
    }

    size_t TreeView::getVisibleCount() const noexcept
    {
        return visibleItems.size();
    }

    size_t TreeView::getMaxCountOfItems() const noexcept
    {
        return rect.height / HEIGHT_OF_ITEM_IN_PIXEL;
    }

    ModelIndex TreeView::getLastClickIndex() const noexcept
    {
        return lastClickedIndex;
    }

    ModelIndex TreeView::getLastHitIndex() const noexcept
    {
        return lastHitIndex;
    }

    TreeView::ItemState TreeView::getItemState(const ModelItem& item) const noexcept
    {
        const auto it = nodeStates.find(item.getUuid());
        if (it == nodeStates.end())
        {
            return ItemState::COLLAPSED;
        }
        return it->second.expanded ? ItemState::EXPANDED : ItemState::COLLAPSED;
    }

    void TreeView::setItemExpanded(const ModelIndex& index, bool expanded) noexcept
    {
        if (!index.isValid()) return;
        const nbstl::Uuid id = index.getUuid();
        auto it = nodeStates.find(id);
        if (it == nodeStates.end())
        {
            nodeStates[id] = NodeState{ expanded, false };
        }
        else
        {
            it->second.expanded = expanded;
        }
        rebuildVisibleList();
    }

    bool TreeView::isItemExpanded(const ModelIndex& index) const noexcept
    {
        if (!index.isValid()) return false;
        const auto it = nodeStates.find(index.getUuid());
        return it != nodeStates.end() && it->second.expanded;
    }

    void TreeView::setItemState(const ModelIndex& index, ItemState state) noexcept
    {
        if (!index.isValid() || !model)
            return;

        const auto* item = model->findById(index.getUuid());
        if (!item)
            return;

        auto& nodeState = nodeStates[item->getUuid()];

        switch (state)
        {
        case ItemState::COLLAPSED:
            nodeState.expanded = false;
            break;
        case ItemState::EXPANDED:
            nodeState.expanded = true;
            break;
        case ItemState::SELECTED:
            // Снимаем выделение со всех, если нужно single-select
            for (auto& [uuid, st] : nodeStates)
                st.selected = false;
            nodeState.selected = true;
            break;
        default:
            break;
        }

        rebuildVisibleList();
    }


    bool ModelItem::haveChildrens() const noexcept
    {
        return !children.empty();
    }

} // namespace Widgets
