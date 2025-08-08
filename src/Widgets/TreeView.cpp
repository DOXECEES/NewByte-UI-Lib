#include "TreeView.hpp"

namespace Widgets
{
	TreeView::TreeView(const NbRect<int>& rect)
		:IWidget(rect)
	{

	}

	

	bool TreeView::hitTest(const NbPoint<int>& pos)
	{
		return false;
	}

	const char* TreeView::getClassName() const
	{
		return "TreeView";
	}


};