#ifndef NBUI_SRC_DOCKMANAGER_HPP
#define NBUI_SRC_DOCKMANAGER_HPP

#include "WindowInterface/IWindow.hpp"
#include "DockTree.hpp"
#include <array>

class Splitter
{
public:
	enum class Placement
	{
		LEFT = 0,
		RIGHT = 1,
		BOT = 2,
		TOP = 3,
		COUNT
	};

	[[nodiscard]] static Placement toSplitterPlacement(DockPlacement placement) noexcept;

	enum class Orientation
	{
		HORIZONTAL = 0x01,
		VERTICAL = 0x02,
		ANY = 0x04,
		COUNT
	};

	[[nodiscard]] static Placement getOpposite(Placement placement) noexcept;

	Splitter()
		: rect()
	{

	}

	void linkNode(const std::shared_ptr<DockNode>& node) noexcept;
	void recalculateRect() noexcept;
	void onMove(const NbPoint<int>& shift)
	{
		if(shift.x > 0)
		{
			rect.x += shift.x;
			
		}
	}

	void update()
	{
		
	}

	void updateHorizontal()
	{
		
	}

	bool hitTest(const NbPoint<int>& pos)
	{
		return pos.x >= rect.x && pos.x < rect.x + rect.width && pos.y >= rect.y && pos.y < rect.y + rect.height;
	}

private:
	bool canMoveVertically(DockNode* node)
	{
		DockPlacement placement = node->getPlacement();

		return placement == DockPlacement::CENTER
			|| placement == DockPlacement::LEFT
			|| placement == DockPlacement::RIGHT;
	}

	bool canMoveHorizontlly(DockNode* node)
	{
		DockPlacement placement = node->getPlacement();

		return placement == DockPlacement::CENTER
			|| placement == DockPlacement::TOP
			|| placement == DockPlacement::BOT;
	}

	

public:


	NbRect<int> rect = {};
	std::vector<std::shared_ptr<DockNode>> linkedNodes;

};

class SplitterContainer
{
public:

	[[nodiscard]] std::shared_ptr<Splitter> createSplitterIfNotExists(Splitter::Placement placement) noexcept
	{
		const size_t index = static_cast<size_t>(Splitter::getOpposite(placement));
		
		if(!container[index])
		{	
			//container[index] = std::make_shared<Splitter>(placement); 
		}

		return container[index];
	}

private:

	std::array<std::shared_ptr<Splitter>, static_cast<size_t>(Splitter::Placement::COUNT)> container;

};

class SplitterManager
{
public:
	
	SplitterManager() noexcept = default;

	void addSplitter(const std::shared_ptr<DockNode>& parent, const std::shared_ptr<DockNode>& insert) noexcept
	{
		// Splitter::Orientation firstWindowOrientation = getSplitterOrientation(parent);
		// Splitter::Orientation secondWindowOrientation = getSplitterOrientation();

		// if(firstWindowOrientation != secondWindowOrientation
		// 	&& firstWindowOrientation != Splitter::Orientation::ANY)
		// {
		// 	Debug::debug("Orientation mismatch");
		// 	abort();
		// }

		Splitter::Placement placement = Splitter::toSplitterPlacement(insert->getPlacement());
		Splitter::Placement oppositePlacement = Splitter::getOpposite(placement);
		std::shared_ptr splitter = std::make_shared<Splitter>();

		splitter->linkNode(insert);
		std::shared_ptr<DockNode> child = parent->getChild(static_cast<DockPlacement>(oppositePlacement));
		if(!child)
		{
			return;
		}

		splitter->linkNode(child);
		splitterList.push_back(splitter);

		//splitterList()	
	}

	void update(const NbPoint<int>& pos)
	{
		static NbPoint<int> prev = { 0, 0 };
		for(auto& splitter : splitterList)
		{
			if(splitter->hitTest(pos))
			{
				splitter->onMove(pos - prev);
			}
		}
		prev = pos;
	}
	
private:
	void calculateSplitterPosition(const std::shared_ptr<DockNode>& first
								 , const std::shared_ptr<DockNode>& second
								 , Splitter::Orientation orientation ) noexcept
	{
		switch(orientation)
		{
			case Splitter::Orientation::HORIZONTAL:
			{

				break;
			}
			case Splitter::Orientation::VERTICAL:
			{
				break;
			}
		}

	}


	Splitter::Orientation getSplitterOrientation(const std::shared_ptr<DockNode>& node) const noexcept
	{
		switch (node->getPlacement())
		{
		case DockPlacement::LEFT:
		case DockPlacement::RIGHT:
			return Splitter::Orientation::VERTICAL;
		case DockPlacement::TOP:
		case DockPlacement::BOT:
			return Splitter::Orientation::HORIZONTAL;
		case DockPlacement::CENTER:
			return Splitter::Orientation::ANY;
		}
	}

private:

	std::vector<std::shared_ptr<Splitter>> splitterList;

	//std::unordered_map<WindowInterface::IWindow*, SplitterContainer> splitterMap;

};


class DockManager
{

public:
	DockManager(WindowInterface::IWindow* window);

	void show();
	void addWindow(WindowInterface::IWindow* parent, WindowInterface::IWindow* dockWindow, DockPlacement placement);

	void update(const std::shared_ptr<DockNode>& parent, DockPlacement placement);
	void onSize(const NbRect<int>& newRect);
	void rescale(const NbSize<float>& scaleFactor);

	void rescaleNode(const std::shared_ptr<DockNode> &node, const NbSize<float> &scaleFactor);
	DockTree* getTree() const noexcept { return tree; } 
	void addSplitter(const std::shared_ptr<DockNode>& insertedNode);

	//inline static std::vector<Splitter*>      splitterList;
private:
	SplitterManager 			splitterManager;

	WindowInterface::IWindow* 	window 	= nullptr;
	DockTree* 					tree 	= nullptr;
};

#endif