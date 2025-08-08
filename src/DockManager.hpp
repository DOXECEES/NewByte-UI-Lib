#ifndef NBUI_SRC_DOCKMANAGER_HPP
#define NBUI_SRC_DOCKMANAGER_HPP

#include "WindowInterface/IWindow.hpp"
#include "DockTree.hpp"
#include <array>

class Splitter
{
public:

	Splitter() = delete;

	Splitter(DockNode* node1, DockNode* node2)
		: firstNode(node1)
		, secondNode(node2)
		, rect()
	{}

	void onMove(const NbPoint<int>& shift)
	{
		// DockPlacement placement = firstNode->getPlacement();
		// NbRect<int> firstRect = firstNode->window->getClientRect();
		// NbRect<int> secondRect = secondNode->window->getClientRect();

		// if (canMoveVertically(firstNode) && canMoveVertically(secondNode))
		// {

		// 	if (firstRect.x < secondRect.x)
		// 	{
		// 		int minWidth = 50;
		// 		int newFirstWidth = firstRect.width + shift.x;
		// 		int newSecondWidth = secondRect.width - shift.x;

		// 		if (newFirstWidth < minWidth || newSecondWidth < minWidth)
		// 			return;

		// 		firstRect.width = newFirstWidth;

		// 		secondRect.x += shift.x;
		// 		secondRect.width = newSecondWidth;
		// 	}
		// 	else
		// 	{
		// 		int minWidth = 50;
		// 		int newSecondWidth = secondRect.width + shift.x;
		// 		int newFirstWidth = firstRect.width - shift.x;

		// 		if (newFirstWidth < minWidth || newSecondWidth < minWidth)
		// 			return;

		// 		secondRect.width = newSecondWidth;

		// 		firstRect.x += shift.x;
		// 		firstRect.width = newFirstWidth;
		// 	}

		// 	firstNode->window->setClientRect(firstRect);
		// 	secondNode->window->setClientRect(secondRect);
		// 	update();

		// }
		// else if (canMoveHorizontlly(firstNode) && canMoveHorizontlly(secondNode))
		// {
		// 	if (firstRect.y < secondRect.y)
		// 	{
		// 		int minHeight = 50;
		// 		int newFirstHeight = firstRect.height + shift.y;
		// 		int newSecondHeight = secondRect.height - shift.y;

		// 		if (newFirstHeight < minHeight || newSecondHeight < minHeight)
		// 			return;

		// 		firstRect.height = newFirstHeight;

		// 		secondRect.y += shift.y;
		// 		secondRect.height = newSecondHeight;
		// 	}
		// 	else
		// 	{
		// 		int minHeight = 50;
		// 		int newFirstHeight = secondRect.height + shift.y;
		// 		int newSecondHeight = firstRect.height - shift.y;

		// 		if (newFirstHeight < minHeight || newSecondHeight < minHeight)
		// 			return;

		// 		secondRect.height = newSecondHeight;

		// 		firstRect.y += shift.y;
		// 		firstRect.height = newFirstHeight;
		// 	}

		// 	firstNode->window->setClientRect(firstRect);
		// 	secondNode->window->setClientRect(secondRect);
		// 	updateHorizontal();
		// }
	}

	void update()
	{
		// const NbRect<int>& firstRect = firstNode->window->getClientRect();
		// const NbRect<int>& secondRect = secondNode->window->getClientRect();

		// if (firstRect.x > secondRect.x)  // left - right
		// {
		// 	rect = {
		// 	   secondRect.x + secondRect.width - 25,
		// 	   secondRect.y,
		// 	   50,
		// 	   firstRect.height
		// 	};
		// }
		// else
		// {
		// 	rect = {
		// 		firstRect.x + firstRect.width - 25,
		// 		firstRect.y,
		// 		50,
		// 		secondRect.height
		// 	};
		// }
	}

	void updateHorizontal()
	{
		// const NbRect<int>& firstRect = firstNode->window->getClientRect();
		// const NbRect<int>& secondRect = secondNode->window->getClientRect();

		// if (firstRect.y > secondRect.y)  // bot - top
		// {
		// 	rect = {
		// 	   secondRect.x,
		// 	   secondRect.y + secondRect.height - 25,
		// 	   secondRect.width,
		// 	   50
		// 	};
		// }
		// else // top - bot
		// {
		// 	rect = {
		// 		firstRect.x,
		// 		firstRect.y + firstRect.height - 25,
		// 		firstRect.width,
		// 		50
		// 	};
		// }
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

	DockNode* firstNode = nullptr;
	DockNode* secondNode = nullptr;

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
	void addSplitter(DockNode* parent, DockNode* node);

	inline static std::vector<Splitter*>      splitterList;
private:
	WindowInterface::IWindow* 	window 	= nullptr;
	DockTree* 					tree 	= nullptr;
};

#endif