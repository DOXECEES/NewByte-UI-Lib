#include "Spacer.hpp"

namespace Widgets
{
	Spacer::Spacer() noexcept
		:IWidget({})
	{}
	
	bool Spacer::hitTest(const NbPoint<int>& pos)
	{
		return false;
	}

	const char* Spacer::getClassName() const
	{
		return "SPACER";
	}
	
	const NbSize<int>& Spacer::measure(const NbSize<int>& max) noexcept
	{
		size = { 0, 0 };
		return size;
	}

	void Spacer::layout(const NbRect<int>& rect) noexcept
	{
		this->rect = rect;
	}

	const NbSize<int>& Spacer::getSize() const noexcept
	{
		return size;
	}
	
};


