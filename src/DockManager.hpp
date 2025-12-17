#ifndef NBUI_SRC_DOCKMANAGER_HPP
#define NBUI_SRC_DOCKMANAGER_HPP

#include "WindowInterface/IWindow.hpp"
#include "DockTree.hpp"
#include <array>




class DockManager
{

public:
	DockManager() = default;
	DockManager(WindowInterface::IWindow* window);

	void show();
	void addWindow(WindowInterface::IWindow* parent, WindowInterface::IWindow* dockWindow, DockPlacement placement);

	void update(const std::shared_ptr<DockNode>& parent, DockPlacement placement);
	void onSize(const NbRect<int>& newRect);
	void rescaleNodeUsingOldRects(const std::shared_ptr<DockNode>& node, const NbSize<float>& scaleFactor);

	DockTree* getTree() const noexcept { return tree; } 

	//inline static std::vector<Splitter*>      splitterList;
private:

	WindowInterface::IWindow* 	window 			= nullptr;
	DockTree* 					tree 			= nullptr;

	NbRect<int>					lastRootRect	= { 0,0,0,0 };

};

#endif