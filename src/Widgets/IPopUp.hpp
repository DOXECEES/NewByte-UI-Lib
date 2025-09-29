#ifndef NBUI_SRC_WIDGETS_IPOPUP_HPP
#define NBUI_SRC_WIDGETS_IPOPUP_HPP

#include "Core.hpp"

// for ListPopUp
#include <vector>
#include <string>

namespace Widgets
{
	class IPopUp
	{
	public:

		IPopUp() noexcept = default;
		virtual ~IPopUp() noexcept = default;

	protected:
		NbRect<int> rect;


	};


	class ListPopUp : public IPopUp
	{
	public:

	private:
		std::vector<std::wstring> listOfText = {
			L"Hello",
			L"HI",
			L"Whatsup",
			L"Sup"
		};
	};
};


#endif 