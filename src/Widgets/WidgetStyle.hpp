#ifndef NBUI_SRC_WIDGETS_WIDGETSTYLE_HPP
#define NBUI_SRC_WIDGETS_WIDGETSTYLE_HPP

#include "../Core.hpp"

struct WidgetStyle
{
	NbColor baseColor		= { 48, 48, 48 };
	NbColor baseTextColor = { 204, 204, 204 };

	NbColor hoverColor = { 64, 64, 64 };
	NbColor hoverTextColor = { 255, 255, 255 };

	NbColor activeColor = { 30, 30, 30 };
	NbColor activeTextColor = { 180, 180, 255 };

	NbColor disableColor = { 70, 70, 70 };
	NbColor disableTextColor = { 120, 120, 120 };
};

#endif