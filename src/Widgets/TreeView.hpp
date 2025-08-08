#ifndef NBUI_SRC_WIDGETS_TREEVIEW_HPP
#define NBUI_SRC_WIDGETS_TREEVIEW_HPP

#include "IWidget.hpp"
#include "DockTree.hpp"

namespace Widgets
{

	class TreeView : public IWidget
	{
	public:

		TreeView(const NbRect<int>& rect);
		~TreeView() {}
		
		void linkTree(DockNode* root) { tree = root; };
		
		virtual bool hitTest(const NbPoint<int>& pos) override;
		virtual const char* getClassName() const override;
		
		DockNode* getTree() { return tree; }

	private:
		DockNode* tree = nullptr;

	};

};


#endif
