#ifndef NBUI_SRC_WIDGETS_TREEVIEW_HPP
#define NBUI_SRC_WIDGETS_TREEVIEW_HPP

#include "IWidget.hpp"

#include "Signal.hpp"

#include <vector>
#include <memory>
#include <functional>

namespace Widgets
{
	class ModelItem;
	class ModelIndex;

}

// Define hash specialization early
namespace std
{
	template<>
	struct hash<Widgets::ModelItem>
	{
		size_t operator()(const Widgets::ModelItem& item) const noexcept;
	};
}

// Now include unordered_map
#include <unordered_map>

namespace Widgets
{

	class ModelIndex
	{
		static constexpr size_t NON_VALID_INDEX = ~0LL;

	public:

		ModelIndex() = default;
		~ModelIndex() = default;

		void setIndex(const size_t newIndex) noexcept
		{
			index = newIndex;
		}

		void setNoIndex() noexcept
		{
			index = NON_VALID_INDEX;
		}

		bool isValid() const noexcept
		{
			return index != NON_VALID_INDEX;
		}

		size_t getRaw() const noexcept
		{
			return index;
		}

		bool operator==(const ModelIndex& other) const noexcept
		{
			return index == other.index;
		}

		bool operator!=(const ModelIndex& other) const noexcept
		{
			return index != other.index;
		}

		friend std::ostream& operator<<(std::ostream& os, const ModelIndex& modelIndex);


	private:
		size_t index = NON_VALID_INDEX;
	};


	inline std::ostream& operator<<(std::ostream& os, const ModelIndex& modelIndex)
	{
		return os << "ModelIndex("
			<< (modelIndex.isValid() ? std::to_string(modelIndex.getRaw()) : "INVALID")
			<< ")";

	}

	//std::ostream& operator<<(std::ostream& os, const ModelIndex& modelIndex);

	class ModelItem
	{
	public:

		ModelItem() = default;
		ModelItem(void* data, ModelItem* itemParent, const size_t itemDepth) 
			: data(data)
			, depth(itemDepth)
			, parent(itemParent)
		{}
		~ModelItem() = default;

		ModelItem(const ModelItem&) = delete;
		ModelItem& operator=(const ModelItem&) = delete;

		ModelItem(ModelItem&&) noexcept = default;
		ModelItem& operator=(ModelItem&&) noexcept = default;

		bool operator==(const ModelItem& other) const noexcept
		{
			return data == other.data && depth == other.depth;
		}

		size_t getDepth() const noexcept;
		size_t getCountOfVisibleChildrens() const noexcept;

		

		void* 									data 					= nullptr;
		ModelItem*								parent					= nullptr;
		std::vector<std::unique_ptr<ModelItem>> children;

		size_t									depth					= 0;

		size_t									countOfVisibleChildrens = 1;
	};

	class ITreeModel
	{
	public:
		const std::vector<std::unique_ptr<Widgets::ModelItem>>& getRootItems() const noexcept { return rootItems; }
		virtual const std::vector<std::unique_ptr<Widgets::ModelItem>>&  getChildren(const ModelItem& parent) const noexcept = 0;

		virtual bool hasChildren(const ModelItem& parent) const noexcept 			= 0;
		virtual std::string data(const ModelItem& item) const noexcept 				= 0;
		virtual std::string data(const ModelIndex& index) noexcept					= 0;

		virtual void forEach(std::function<void(const ModelItem&)> func) noexcept 	= 0;

		const ModelItem& findByIndex(const ModelIndex& index) noexcept;

		size_t getSize() noexcept;

	private:
		void sizeCalculateForce() noexcept;

	protected:
		std::vector<std::unique_ptr<Widgets::ModelItem>> rootItems;
	
	private:
		size_t	size;

		bool	isSizeChanged = true;
	};


	class TreeView : public IWidget
	{
	public:
		static constexpr const char*	CLASS_NAME				= "TreeView";
		static constexpr size_t			HEIGHT_OF_ITEM_IN_PIXEL = 20;
		enum class ItemState
		{
			EXPANDED,
			COLLAPSED,
			EXPANDED_ACTIVE,
			LEAF,
			UNACTIVE,
		};

		TreeView(const NbRect<int>& rect);
		~TreeView() = default;

		bool hitTest(const NbPoint<int>& pos) override;
		bool hitTestClick(const NbPoint<int>& pos) noexcept override;

		size_t hitElement(const NbPoint<int>& pos) const noexcept;
		const char* getClassName() const override;
		
		void setModel(const std::shared_ptr<ITreeModel>& modelParam) noexcept;
		inline std::shared_ptr<ITreeModel> getModel() const noexcept { return model; }

		void onItemClick(const std::shared_ptr<ModelItem>& item) noexcept;
		//void onItemExpand(std::shared_ptr<TreeItem> item) noexcept;
		//void onItemCollapse(std::shared_ptr<TreeItem> item) noexcept;
		//void onSelectionChanged(std::shared_ptr<TreeItem> item) noexcept;

		size_t getLastHitIndex() const noexcept;
		ModelIndex getLastClickedIndex() const noexcept;

		ItemState getItemState(const ModelItem& item) const noexcept;
		void setItemState(const ModelIndex& index, ItemState state) noexcept;

		size_t getMaxCountOfItems() const noexcept;


	public: // signals
		Signal<void(ModelIndex)> onItemClickSignal;


	protected:
		void calculateVisibleForce() noexcept;

	private:

		//std::vector<const ModelItem*> visibleTree;

		size_t lastHitIndex = 0;
		ModelIndex lastClickedIndex; 

		std::pair<size_t, size_t> range = { 0 , this->rect.height };

		std::shared_ptr<ITreeModel> model = nullptr;
		mutable std::unordered_map<const ModelItem*, ItemState> stateMap;
	};

};



#endif
