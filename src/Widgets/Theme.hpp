#ifndef NBUI_SRC_WIDGETS_THEME_HPP
#define NBUI_SRC_WIDGETS_THEME_HPP

#include "WidgetStyle.hpp"


class ThemeManager
{
public:
	
	struct Theme
	{
		WidgetStyle		widgetStyle;
		TreeViewStyle	treeViewStyle;
		ButtonStyle		buttonStyle;
		TextEditStyle	textEditStyle;
		LabelStyle		labelStyle;
		CheckBoxStyle	checkBoxStyle;
	};

	static const Theme& getCurrent() noexcept;

private:
	inline static Theme theme;
};


#endif