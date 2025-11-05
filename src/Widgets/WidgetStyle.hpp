#ifndef NBUI_SRC_WIDGETS_WIDGETSTYLE_HPP
#define NBUI_SRC_WIDGETS_WIDGETSTYLE_HPP

#include "../Core.hpp"

// Общий стиль для всех виджетов
struct WidgetStyle
{
	NbColor baseColor = { 37, 37, 38 };	 // фон
	NbColor baseTextColor = { 220, 220, 220 }; // обычный текст

	NbColor hoverColor = { 62, 62, 64 };	 // при наведении
	NbColor hoverTextColor = { 255, 255, 255 }; // текст при наведении

	NbColor activeColor = { 0, 122, 204 };	 // активный (нажатый) элемент
	NbColor activeTextColor = { 255, 255, 255 }; // активный текст

	NbColor disableColor = { 55, 55, 55 };	 // отключённый элемент
	NbColor disableTextColor = { 100, 100, 100 }; // отключённый текст
};

// Стиль для TreeView
struct TreeViewStyle
{
	NbColor selectionColor = { 51, 153, 255 };  // выделение (VS style)
	NbColor hoverSelectionColor = { 75, 165, 255 };  // выделение при наведении
	NbColor clickColor = { 0, 122, 204 };   // активный элемент
	NbColor buttonColor = { 60, 60, 60 };    // кнопки раскрытия
	NbColor inButtonColor = { 100, 100, 100 }; // при наведении на кнопку
};

// Кнопки — слегка ярче, чтобы выделяться на фоне
struct ButtonStyle
{
	// 🎨 Основные состояния
	NbColor baseColor = { 45, 45, 48 };	 // обычное состояние
	NbColor baseTextColor = { 220, 220, 220 }; // обычный текст

	NbColor hoverColor = { 63, 63, 70 };	 // наведение
	NbColor hoverTextColor = { 255, 255, 255 }; // текст при наведении

	NbColor activeColor = { 0, 122, 204 };	 // нажата
	NbColor activeTextColor = { 255, 255, 255 }; // текст при нажатии

	NbColor disableColor = { 60, 60, 60 };	 // выключена
	NbColor disableTextColor = { 120, 120, 120 }; // текст выключенной кнопки

	// ✨ Дополнительные элементы оформления
	NbColor borderColor = { 90, 90, 90 };	 // цвет рамки
	NbColor focusColor = { 70, 70, 80 };	 // при фокусе клавиатурой
	int borderRadius = 4;				 // скругление углов
};

// Поля ввода
struct TextEditStyle
{
	NbColor backgroundColor = { 30, 30, 30 };
	NbColor borderColor = { 70, 70, 70 };
	NbColor hoverBorderColor = { 100, 100, 100 };
	NbColor focusBorderColor = { 0, 122, 204 };
	NbColor textColor = { 240, 240, 240 };
	NbColor placeholderColor = { 130, 130, 130 };
};

// Метки и текст
struct LabelStyle
{
	NbColor textColor = { 220, 220, 220 };
};

// Чекбоксы
struct CheckBoxStyle
{
	NbColor boxColor = { 50, 50, 50 };
	NbColor checkColor = { 0, 122, 204 };
	NbColor hoverColor = { 70, 70, 70 };
	NbColor disableColor = { 60, 60, 60 };
	NbColor borderColor = { 90, 90, 90 };
	NbColor textColor = { 220, 220, 220 };
};

#endif
