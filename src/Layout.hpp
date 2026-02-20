#ifndef NBUI_SRC_LAYOUT_HPP
#define NBUI_SRC_LAYOUT_HPP
#define NOMINMAX

#include "Core.hpp"


#include <DynamicFlatmatrix.hpp>

#include "WindowInterface/IWindow.hpp"
#include "Widgets/IWidget.hpp"
#include "Widgets/Indentations.hpp"

#include <vector>

template<typename T>
struct OldMargin
{
    T left  = static_cast<T>(0);
    T right = static_cast<T>(0);
    T top   = static_cast<T>(0);
    T bot   = static_cast<T>(0);
};


struct LayoutParams
{
    NbSize<int> preferred;

    NbSize<float> percent;

    NbSize<int> min;
    NbSize<int> max;

    float grow = 0.0f;  
    float shrink = 1.0f; 

    Margin<int> margin;

    enum class Align
    {
        Start, Center, End, Stretch
    };

    Align alignX = Align::Start;
    Align alignY = Align::Start;

    float aspectRatio = 0.0f;

};


class Layout : WindowInterface::IWindowStateChangedListener
{
public:
    Layout(WindowInterface::IWindow* window)
        : window(window)
    {
        window->addStateChangedListener(this);
    }

    virtual ~Layout() = default;

    void onSizeChanged(const NbSize<int>& size) override
    {
        caclulateLayout();
    }

    void addWidget(Widgets::IWidget* widget)
    {
        window->linkWidget(widget);
        linkedWidgets.push_back(widget);

        for (auto& child : widget->getChildrens())
            window->linkWidget(child);
    }

    void addLayout(Layout* layout)
    {
        layout->setWindow(window); // привязываем вложенный layout к тому же окну
        linkedLayouts.push_back(layout);
    }

    void setSpacing(int s) noexcept { spacing = s; }

    // --- Прямоугольник для layout (для рекурсивного размещения) ---
    void setRect(const NbRect<int>& r) noexcept { rect = r; }
    const NbRect<int>& getRect() const noexcept { return rect; }

    // --- Requested size для flex (можно возвращать размер контента или {0,0}) ---
    virtual NbSize<int> getRequestedSize() const noexcept { return {}; }

//protected:
    virtual void caclulateLayout() noexcept = 0;

    void setWindow(WindowInterface::IWindow* w) noexcept { window = w; }

    WindowInterface::IWindow* window = nullptr;
    std::vector<Widgets::IWidget*> linkedWidgets;
    std::vector<Layout*> linkedLayouts;
    int spacing = 5;
    NbRect<int> rect;
};



class VBoxLayout : public Layout
{
public:
    VBoxLayout(WindowInterface::IWindow *window) : Layout(window) {}

	void caclulateLayout() noexcept override
	{
		if (linkedWidgets.empty()) return;

		auto clientRect = applyPaddingToRect(window->getClientRect(), OldPadding());
		clientRect.y += 35;
		int currentY = 5;

		int fixedHeight = 0;
		int flexibleCount = 0;

		for (auto& widget : linkedWidgets)
		{
			if (!widget->getRequestedSize().isEmpty())
				fixedHeight += widget->getRequestedSize().height;
			else
				flexibleCount++;
		}

		int remainingHeight = clientRect.height - fixedHeight;
		int flexHeight = (flexibleCount > 0) ? remainingHeight / flexibleCount : 0;

		for (auto& widget : linkedWidgets)
		{
			NbRect<int> rect;
			if (!widget->getRequestedSize().isEmpty())
			{
				rect = widget->getRequestedSize();
				rect.x += 5;
				rect.y = window->getFrameSize().top + currentY;
				rect.width = clientRect.width;
			}
			else
			{
				rect = { 5, window->getFrameSize().top + currentY, clientRect.width, flexHeight };
			}

			//rect = applyHeightOnlyPaddingToRect(rect, Padding());
			widget->setRect(rect);
			currentY += rect.height;
		}
	}

};

class GridLayout : public Layout
{
public:
    GridLayout(WindowInterface::IWindow* window, const size_t rows, const size_t cols)
        : Layout(window)
        , columns(cols)
        , rows(rows)
        , grid(rows, cols)
    {
    }


    void addWidget(Widgets::IWidget* widget, const size_t row, const size_t column) noexcept
    {
        if (row >= rows || column >= columns)
            return;

        grid.insert(widget, row, column);
        window->linkWidget(widget);
        if (std::find(linkedWidgets.begin(), linkedWidgets.end(), widget) == linkedWidgets.end())
            linkedWidgets.push_back(widget);
    }

protected:
    void caclulateLayout() noexcept override
    {
        if (linkedWidgets.empty())
            return;

        auto clientRect = window->getClientRect();
        WindowInterface::FrameSize frameSize = window->getFrameSize();

        const int totalHPadding = static_cast<int>(columns - 1) * cellPadding.width;
        const int totalVPadding = static_cast<int>(rows - 1) * cellPadding.height;

        // --- Этап 1: вычисляем размеры строк и колонок ---
        std::vector<int> rowHeights(rows, 0);
        std::vector<int> columnWidths(columns, 0);
        std::vector<bool> rowHasExpanding(rows, false);
        std::vector<bool> colHasExpanding(columns, false);

        for (size_t row = 0; row < rows; ++row)
        {
            for (size_t col = 0; col < columns; ++col)
            {
                auto* widget = grid.get(row, col);
                if (!widget) continue;

                const auto policy = widget->getSizePolicy();
                const auto req = widget->getRequestedSize();

                if (policy.horizontal == Widgets::SizePolicy::EXPANDING || policy.horizontal == Widgets::SizePolicy::STRETCH)
                    colHasExpanding[col] = true;
                if (policy.vertical == Widgets::SizePolicy::EXPANDING || policy.vertical == Widgets::SizePolicy::STRETCH)
                    rowHasExpanding[row] = true;

                columnWidths[col] = (std::max)(columnWidths[col], req.width);
                rowHeights[row] = (std::max)(rowHeights[row], req.height);
            }
        }

        // --- Этап 2: перераспределяем ширину для растягивающихся колонок ---
        int totalFixedWidth = 0;
        int expandingCols = 0;
        for (size_t col = 0; col < columns; ++col)
        {
            if (colHasExpanding[col])
                expandingCols++;
            else
                totalFixedWidth += columnWidths[col];
        }

        int remainingWidth = clientRect.width - totalFixedWidth - totalHPadding;
        int extraWidthPerCol = expandingCols > 0 ? remainingWidth / expandingCols : 0;
        int leftoverWidth = expandingCols > 0 ? remainingWidth % expandingCols : 0;

        std::vector<int> finalColumnWidths(columns, 0);
        for (size_t col = 0; col < columns; ++col)
        {
            if (colHasExpanding[col])
            {
                finalColumnWidths[col] = extraWidthPerCol;
                if (leftoverWidth > 0)
                {
                    finalColumnWidths[col] += 1;
                    leftoverWidth--;
                }
            }
            else
            {
                finalColumnWidths[col] = columnWidths[col];
            }
        }

        // --- Этап 3: перераспределяем высоту для растягивающихся строк ---
        int totalFixedHeight = 0;
        int expandingRows = 0;
        for (size_t row = 0; row < rows; ++row)
        {
            if (rowHasExpanding[row])
                expandingRows++;
            else
                totalFixedHeight += rowHeights[row];
        }

        int remainingHeight = clientRect.height - totalFixedHeight - totalVPadding;
        int extraHeightPerRow = expandingRows > 0 ? remainingHeight / expandingRows : 0;

        // --- Этап 4: размещаем виджеты ---
        int y = clientRect.y + frameSize.top;
        for (size_t row = 0; row < rows; ++row)
        {
            int rowHeight = rowHasExpanding[row] ? rowHeights[row] + extraHeightPerRow : rowHeights[row];
            int x = clientRect.x + frameSize.left;

            for (size_t col = 0; col < columns; ++col)
            {
                auto* widget = grid.get(row, col);
                int colWidth = finalColumnWidths[col];

                if (!widget)
                {
                    x += colWidth + cellPadding.width;
                    continue;
                }

                const auto policy = widget->getSizePolicy();
                const auto req = widget->getRequestedSize();

                int w = colWidth;
                int h = rowHeight;

                // Горизонтальная политика
                switch (policy.horizontal)
                {
                case Widgets::SizePolicy::FIXED:
                    w = req.width;
                    break;
                case Widgets::SizePolicy::MINIMUM:
                case Widgets::SizePolicy::PREFERRED:
                    w = (std::min)(req.width, colWidth);
                    break;
                case Widgets::SizePolicy::EXPANDING:
                case Widgets::SizePolicy::STRETCH:
                    w = colWidth;
                    break;
                }

                // Вертикальная политика
                switch (policy.vertical)
                {
                case Widgets::SizePolicy::FIXED:
                    h = req.height;
                    break;
                case Widgets::SizePolicy::MINIMUM:
                case Widgets::SizePolicy::PREFERRED:
                    h = (std::min)(req.height, rowHeight);
                    break;
                case Widgets::SizePolicy::EXPANDING:
                case Widgets::SizePolicy::STRETCH:
                    h = rowHeight;
                    break;
                }

                // Центрирование
                int offsetX = (colWidth - w) / 2;
                int offsetY = (rowHeight - h) / 2;

                widget->setRect({ x + offsetX, y + offsetY, w, h });

                x += colWidth + cellPadding.width;
            }

            y += rowHeight + cellPadding.height;
        }
    }

private:
    size_t columns;
    size_t rows;
    nbstl::DynamicFlatMatrix<Widgets::IWidget*> grid;
    NbSize<int> cellPadding = { 5, 5 };

    int desiredHeight   = 1;
    int desiredWidth    = 1;
};

class FlexLayout : public Layout
{
public:
    enum class Direction { Row, Column };

    FlexLayout(WindowInterface::IWindow* window, Direction dir = Direction::Column)
        : Layout(window), direction(dir) {
    }

    void setDirection(Direction dir) noexcept { direction = dir; }

//protected:
    void caclulateLayout() noexcept override
    {
        NbRect<int> r = rect;
        if (r.isEmpty())
        {
            r = window->getClientRect();
        }
        const auto& frame = window->getFrameSize();

        struct Element 
        { 
            Widgets::IWidget* w = nullptr;
            Layout* l = nullptr;
        };
        
        std::vector<Element> elements;
        for (auto& w : linkedWidgets)
        {
            elements.emplace_back(w, nullptr );
        }

        for (auto& l : linkedLayouts)
        {
            elements.emplace_back( nullptr, l );
        }

        if (elements.empty())
        {
            return;
        }
        // --- Этап 1: считаем фиксированные и flexible размеры ---
        int totalFixed = 0;
        int flexibleCount = 0;

        for (auto& e : elements)
        {
            NbRect<int> reqRect;
            Widgets::SizePolicy policyH = Widgets::SizePolicy::FIXED;
            Widgets::SizePolicy policyV = Widgets::SizePolicy::FIXED;

            if (e.w)
            {
                reqRect = e.w->getRequestedSize();
                auto pol = e.w->getSizePolicy();
                policyH = pol.horizontal;
                policyV = pol.vertical;
            }
            else if (e.l)
            {
                reqRect = { 0, 0, e.l->getRequestedSize().width, e.l->getRequestedSize().height };
                // layout-ы можно считать EXPANDING по умолчанию
                policyH = policyV = Widgets::SizePolicy::EXPANDING;
            }

            bool flexible = false;
            if (direction == Direction::Row)
                flexible = (policyH == Widgets::SizePolicy::EXPANDING || policyH == Widgets::SizePolicy::STRETCH) && reqRect.isEmpty();
            else
                flexible = (policyV == Widgets::SizePolicy::EXPANDING || policyV == Widgets::SizePolicy::STRETCH) && reqRect.isEmpty();

            if (flexible)
            {
                flexibleCount++;
            }
            else
            {
                totalFixed += (direction == Direction::Row ? reqRect.width : reqRect.height);
            }
        }

        int spacingTotal = spacing * (elements.size() - 1);
        int remainingSpace = (direction == Direction::Row ? r.width : r.height) - totalFixed - spacingTotal;
        int flexSize = (flexibleCount > 0 ? remainingSpace / flexibleCount : 0);

        // --- Этап 2: размещаем элементы ---
        int offset = 0;
        for (auto& e : elements)
        {
            NbRect<int> rectElem;
            NbRect<int> reqRect;
            Widgets::SizePolicy policyH = Widgets::SizePolicy::FIXED;
            Widgets::SizePolicy policyV = Widgets::SizePolicy::FIXED;

            if (e.w)
            {
                reqRect = e.w->getRequestedSize();
                auto pol = e.w->getSizePolicy();
                policyH = pol.horizontal;
                policyV = pol.vertical;
            }
            else if (e.l)
            {
                reqRect = { 0, 0, e.l->getRequestedSize().width, e.l->getRequestedSize().height };
                policyH = policyV = Widgets::SizePolicy::EXPANDING;
            }

            // --- вычисляем размер ---
            if (!reqRect.isEmpty())
            {
                rectElem = reqRect;
            }
            else
            {
                rectElem = (direction == Direction::Row) ? NbRect<int>{0, 0, flexSize, r.height} : NbRect<int>{ 0,0,r.width,flexSize };
            }

            // --- применяем политику ---
            if (direction == Direction::Row)
            {
                switch (policyH)
                {
                    case Widgets::SizePolicy::FIXED: rectElem.width = reqRect.width; break;
                    case Widgets::SizePolicy::MINIMUM:
                    case Widgets::SizePolicy::PREFERRED: rectElem.width = (std::min)(reqRect.width, flexSize); break;
                    case Widgets::SizePolicy::EXPANDING:
                    case Widgets::SizePolicy::STRETCH: rectElem.width = flexSize; break;
                }
                rectElem.x = r.x + offset;
                rectElem.y = r.y + frame.top;
                offset += rectElem.width + spacing;
            }
            else
            {
                switch (policyV)
                {
                    case Widgets::SizePolicy::FIXED: rectElem.height = reqRect.height; break;
                    case Widgets::SizePolicy::MINIMUM:
                    case Widgets::SizePolicy::PREFERRED: rectElem.height = (std::min)(reqRect.height, flexSize); break;
                    case Widgets::SizePolicy::EXPANDING:
                    case Widgets::SizePolicy::STRETCH: rectElem.height = flexSize; break;
                }
                rectElem.x = r.x + frame.left;
                rectElem.y = r.y + frame.top + offset;
                offset += rectElem.height + spacing;
            }

            // --- назначаем rect элементу ---
            if (e.w) e.w->setRect(rectElem);
            else if (e.l)
            {
                rectElem.y -= frame.top;
                e.l->setRect(rectElem);
                e.l->caclulateLayout(); // рекурсивно раскладываем вложенные layout-ы
            }
        }
    }

private:
    Direction direction;
};




#endif


