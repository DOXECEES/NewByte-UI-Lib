#include "TreeView.hpp"

#include "Debug.hpp"

namespace Widgets
{
	TreeView::TreeView(const NbRect<int>& rect)
		: IWidget(rect)
	{
		//visibleTree.reserve(getMaxCountOfItems());
	}

	bool TreeView::hitTest(const NbPoint<int>& pos)
	{
		lastHitIndex = hitElement(pos);
		return false;
	}

	bool TreeView::hitTestClick(const NbPoint<int>& pos) noexcept
	{
		if (!model)
		{
			return false;
		}

		lastClickedIndex.setIndex(hitElement(pos));// tech dept is too high

		if (lastClickedIndex.getRaw() > model->getSize() - 1)
			lastClickedIndex.setNoIndex();


		onItemClickSignal.emit(lastClickedIndex);
		return false;  // some piece of shit 
					   // need refactoring
	}

	size_t TreeView::hitElement(const NbPoint<int>& pos) const noexcept
	{
		size_t elementIndex = range.first + pos.y;
		return elementIndex / HEIGHT_OF_ITEM_IN_PIXEL;
	}

	const char* TreeView::getClassName() const
	{
		return CLASS_NAME;
	}


	void TreeView::setModel(const std::shared_ptr<ITreeModel>& modelParam) noexcept
	{
		model = modelParam;
		model->forEach([this](const ModelItem& item) {

			auto p = &item;
			stateMap[p] = ItemState::EXPANDED;
		});

		calculateVisibleForce();
	}

	void TreeView::onItemClick(const std::shared_ptr<ModelItem>& item) noexcept
	{

	}

	size_t TreeView::getLastHitIndex() const noexcept
	{
		return lastHitIndex;
	}

	ModelIndex TreeView::getLastClickedIndex() const noexcept
	{
		return lastClickedIndex;
	}

	Widgets::TreeView::ItemState TreeView::getItemState(const ModelItem& item) const noexcept
	{
		if (stateMap.find(&item) == stateMap.end())
		{
			stateMap[&item] = ItemState::COLLAPSED;
		}

		return stateMap.at(&item);
	}

	void TreeView::setItemState(const ModelIndex& index, ItemState state) noexcept
	{
		std::optional<std::reference_wrapper<const ModelItem>> item = model->findByIndex(index);
		stateMap[&item->get()] = state;
		calculateVisibleForce();
	}

	size_t TreeView::getMaxCountOfItems() const noexcept
	{
		return rect.height / HEIGHT_OF_ITEM_IN_PIXEL;
	}

	void TreeView::calculateVisibleForce() noexcept
	{
		struct Frame
		{
			ModelItem*	node;
			size_t		idx;   
			size_t		total; 
		};

		std::vector<Frame> stack;

		for (auto& root : model->getRootItems())
		{
			stack.push_back({ root.get(), 0, 1 });

			while (!stack.empty())
			{
				Frame& frame = stack.back();

				if (frame.idx < frame.node->children.size())
				{
					stack.push_back({ frame.node->children[frame.idx].get(), 0, 1 });
					frame.idx++;
				}
				else
				{
					if (stateMap[frame.node] == ItemState::EXPANDED)
					{
						for (auto& child : frame.node->children)
						{
							frame.total += child->countOfVisibleChildrens - 1;
						}
					}
					frame.node->countOfVisibleChildrens = frame.total;

					stack.pop_back();

					if (!stack.empty())
					{
						stack.back().total += frame.total;
					}
				}
			}
		}

	}

	size_t ModelItem::getDepth() const noexcept
	{
		return depth;
	}

	size_t ModelItem::getCountOfVisibleChildrens() const noexcept
	{
		return countOfVisibleChildrens;
	}

	std::optional<std::reference_wrapper<const Widgets::ModelItem>> ITreeModel::findByIndex(const ModelIndex& index) noexcept
	{
		size_t rawIndex = index.getRaw();
		size_t localIndex = 0;

		const ModelItem* ptrItem = nullptr;

		forEach([&rawIndex, &localIndex, &ptrItem](const ModelItem& item) {
			if (localIndex == rawIndex)
			{
				ptrItem = &item;
			}
			localIndex++;
		});

		if (!ptrItem)
		{
			return std::nullopt;
		}

		return std::cref(*ptrItem);
	}

	size_t ITreeModel::getSize() noexcept
	{
		if (!isSizeChanged)
		{
			return size;
		}

		sizeCalculateForce();
		isSizeChanged = false;
		return size;
	}

	void ITreeModel::sizeCalculateForce() noexcept
	{
		size = 0;
		forEach([this](const ModelItem& item) {
			size++;
		});
	}

};

namespace std
{
	size_t hash<Widgets::ModelItem>::operator()(const Widgets::ModelItem& item) const noexcept
	{
		size_t h1 = std::hash<void*>{}(item.data);
		size_t h2 = std::hash<size_t>{}(item.depth);
		return h1 ^ (h2 << 1);
	}
}