// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
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
        if (!rect.isInside(pos))
        {
            lastHitIndex = ModelIndex{};
            return false;
        }

        int localY = pos.y - rect.y;

        size_t absoluteRow = (scrollOffsetY + rect.y + localY) / HEIGHT_OF_ITEM_IN_PIXEL;

        if (absoluteRow >= range.first)
        {
            size_t relativeIdx = absoluteRow - range.first;

            if (relativeIdx < visibleItems.size())
            {
                lastHitIndex = ModelIndex(visibleItems[relativeIdx]->getUuid());
                return true;
            }
        }

        lastHitIndex = ModelIndex{};
        return false;
    }

    size_t TreeView::hitElement(const NbPoint<int>& pos) const noexcept
    {

        int localY = pos.y - rect.y;

        size_t absoluteY = static_cast<size_t>(range.first + localY);

        return absoluteY / HEIGHT_OF_ITEM_IN_PIXEL;
    }


    bool TreeView::hitTestClick(const NbPoint<int>& pos) noexcept
    {
        if (!model || !rect.isInside(pos))
        {
            return false;
        }

        NbPoint<int> localPos = {pos.x - rect.x, pos.y - rect.y};
        size_t       row      = (localPos.y + range.first) / HEIGHT_OF_ITEM_IN_PIXEL;

        ModelIndex clickedIndex = indexFromVisibleRow(row);
        if (!clickedIndex.isValid())
        {
            return false;
        }

        setItemState(clickedIndex, ItemState::SELECTED);

        static ModelIndex prevClickedIndex;
        lastClickedIndex = clickedIndex;

        const ModelItem* item = uuidMap.at(lastClickedIndex.getUuid());

        int         buttonLocalY    = (int)(row * HEIGHT_OF_ITEM_IN_PIXEL) - range.first;
        int         buttonLocalX    = (int)20 * (int)item->getDepth();
        NbRect<int> buttonRectLocal = {buttonLocalX, buttonLocalY, 20, 20};

        if (isItemHaveChildrens(lastClickedIndex) && buttonRectLocal.isInside(localPos))
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

        prevClickedIndex = lastClickedIndex;
        return true;
    }


    bool TreeView::hitTestRightClick(const NbPoint<int>& pos) noexcept
    {
        if (!model || !rect.isInside(pos))
        {
            return false;
        }

        NbPoint<int> localPos = {pos.x - rect.x, pos.y - rect.y};
        size_t       row      = (localPos.y + range.first) / HEIGHT_OF_ITEM_IN_PIXEL;

        ModelIndex clickedIndex = indexFromVisibleRow(row);
        if (!clickedIndex.isValid())
        {
            return false;
        }

        setItemState(clickedIndex, ItemState::SELECTED);
        lastClickedIndex = clickedIndex;

        onItemRightClickSignal.emit(lastClickedIndex);
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

        for (const auto& [uuid, value] : uuidMap) 
        {
            nodeStates.try_emplace(uuid, false, false);
        }

        for (const auto& rootPtr : model->getRootItems())
        {
            nodeStates[rootPtr->getUuid()].expanded = true;
        }

        rebuildVisibleList();
    }

    void TreeView::refresh() noexcept
    {
        if (!model)
        {
            return;
        }

        uuidMap.clear();
        visibleItems.clear();

        buildUuidMap();

        std::unordered_map<nbstl::Uuid, NodeState> newNodeStates;
        for (const auto& [uuid, value] : uuidMap)
        {
            auto it = nodeStates.find(uuid);
            if (it != nodeStates.end())
            {
                newNodeStates[uuid] = it->second;
            }
            else
            {
                newNodeStates.try_emplace(uuid, false, false);
            }
        }

        nodeStates = std::move(newNodeStates);

        rebuildVisibleList();
    }

    void TreeView::renameItem(
        const ModelIndex& index,
        const std::string& name
    ) noexcept
    {
        if (!index.isValid() || !model)
        {
            return;
        }

        model->setData(index.getUuid(), name);
        onItemChangeSignal.emit(index);

        rebuildVisibleList();
    }

    void TreeView::setSelectedItem(const ModelIndex& index) noexcept
    {
        if (!index.isValid() || !model)
        {
            return;
        }

        auto itTarget = uuidMap.find(index.getUuid());
        if (itTarget == uuidMap.end())
        {
            return;
        }

        for (auto& [uuid, state] : nodeStates)
        {
            state.selected = false;
        }

        nodeStates[index.getUuid()].selected = true;

        const ModelItem* current = itTarget->second;
        if (current)
        {
            ModelItem* parent = current->parent;
            while (parent)
            {
                nodeStates[parent->getUuid()].expanded = true;
                parent                                 = parent->parent;
            }
        }

        rebuildVisibleList();
        onItemChangeSignal.emit(index);
    }

    void TreeView::buildUuidMap() noexcept
    {
        if (!model)
        {
            return;
        }

        model->forEach([this](const ModelItem& item)
        {
            uuidMap.try_emplace( item.getUuid(), &item );
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

    void TreeView::startEditing(const ModelIndex& index) noexcept
    {
        if (!index.isValid() || !model)
        {
            return;
        }

        const auto* item = model->findById(index.getUuid());
        if (!item)
        {
            return;
        }

        editingIndex = index;
        editingText = model->data(*item);
        isEditing = true;
    }

    void TreeView::commitEditing() noexcept
    {
        if (!isEditing)
        {
            return;
        }

        renameItem(editingIndex, editingText);

        isEditing = false;
    }

    void TreeView::cancelEditing() noexcept
    {
        isEditing = false;
    }

    void TreeView::inputChar(char c) noexcept
    {
        if (!isEditing)
        {
            return;
        }

        if (c < 32)
        {
            return;
        }

        editingText += c;
    }

    void TreeView::backspace() noexcept
    {
        if (!isEditing || editingText.empty())
        {
            return;
        }

        editingText.pop_back();
    }

    bool TreeView::isEditingItem(const ModelIndex& index) const noexcept
    {
        return isEditing && index == editingIndex;
    }

    const std::string& TreeView::getEditingText() const noexcept
    {
        return editingText;
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
            for (auto& [uuid, st] : nodeStates)
            {
                st.selected = false;
            }
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
