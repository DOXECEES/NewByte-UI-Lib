#ifndef NBUI_SRC_WIDGETS_WIDGETSTYLE_HPP
#define NBUI_SRC_WIDGETS_WIDGETSTYLE_HPP

#include <NbCore.hpp>
#include "../Core.hpp"

struct Border
{
    enum class Style
    {
        NONE,      // Нет границы
        SOLID,     // Сплошная линия
        DASHED,    // Пунктирная
        DOTTED,    // Точечная
        DOUBLE,    // Двойная линия
        GROOVE,    // 3D-желоб
        RIDGE,     // 3D-ребро
        INSET,     // Вдавленная
        OUTSET     // Выпуклая
    };

    Style style = Style::NONE;
    int width = 1;
    int radius = 0;
    NbColor color;
};


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

    Border  border;
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
    // Компонент для визуальных состояний
    WidgetStyle baseStyle
    {
        .baseColor = {45, 45, 48},
        .baseTextColor = {220, 220, 220},
        .hoverColor = {63, 63, 70},
        .hoverTextColor = {255, 255, 255},
        .activeColor = {0, 122, 204},
        .activeTextColor = {255, 255, 255},
        .disableColor = {60, 60, 60},
        .disableTextColor = {120, 120, 120}
    };

    NbColor borderColor = { 90, 90, 90 };
    NbColor focusColor = { 70, 70, 80 };
    int borderRadius = 4;

    NB_NODISCARD NbColor& baseColor() noexcept { return baseStyle.baseColor; }
    NB_NODISCARD const NbColor& baseColor() const noexcept { return baseStyle.baseColor; }

    NB_NODISCARD NbColor& baseTextColor() noexcept { return baseStyle.baseTextColor; }
    NB_NODISCARD const NbColor& baseTextColor() const noexcept { return baseStyle.baseTextColor; }

    NB_NODISCARD NbColor& hoverColor() noexcept { return baseStyle.hoverColor; }
    NB_NODISCARD const NbColor& hoverColor() const noexcept { return baseStyle.hoverColor; }

    NB_NODISCARD NbColor& hoverTextColor() noexcept { return baseStyle.hoverTextColor; }
    NB_NODISCARD const NbColor& hoverTextColor() const noexcept { return baseStyle.hoverTextColor; }

    NB_NODISCARD NbColor& activeColor() noexcept { return baseStyle.activeColor; }
    NB_NODISCARD const NbColor& activeColor() const noexcept { return baseStyle.activeColor; }

    NB_NODISCARD NbColor& activeTextColor() noexcept { return baseStyle.activeTextColor; }
    NB_NODISCARD const NbColor& activeTextColor() const noexcept { return baseStyle.activeTextColor; }

    NB_NODISCARD NbColor& disableColor() noexcept { return baseStyle.disableColor; }
    NB_NODISCARD const NbColor& disableColor() const noexcept { return baseStyle.disableColor; }

    NB_NODISCARD NbColor& disableTextColor() noexcept { return baseStyle.disableTextColor; }
    NB_NODISCARD const NbColor& disableTextColor() const noexcept { return baseStyle.disableTextColor; }

    NB_NODISCARD Border& border() noexcept { return baseStyle.border; }
    NB_NODISCARD const Border& border() const noexcept { return baseStyle.border; }

    void updateFrom(const WidgetStyle& ws) noexcept
    {
        baseStyle = ws;
    }
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
