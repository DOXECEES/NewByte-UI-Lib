#ifndef NBUI_SRC_WIDGETS_ILAYOUTABLE_HPP
#define NBUI_SRC_WIDGETS_ILAYOUTABLE_HPP

namespace Widgets
{
	class ILayoutable
	{
		virtual Size measure(const Constraints&) noexcept = 0;
		virtual void arrange(const Rect&) noexcept = 0;
	};
};

#endif