#ifndef NBUI_SRC_LAYOUT_HPP
#define NBUI_SRC_LAYOUT_HPP

#include "Core.hpp"

#include "WindowInterface/IWindow.hpp"
#include "Widgets/IWidget.hpp"
#include "Widgets/Indentations.hpp"

#include <vector>




class Layout : WindowInterface::IWindowStateChangedListener
{
public:
    Layout(WindowInterface::IWindow *window)
        :window(window)
    {
        window->addStateChangedListener(this);
    }

    virtual ~Layout() = default;

    void onSizeChanged(const NbSize<int>& size) override
    {
        caclulateLayout();
    }

    void addWidget(Widgets::IWidget *widget)
    {
        window->linkWidget(widget);
        linkedWidgets.push_back(widget);
    }

    void addLayout(Layout *layout)
    {
        linkedLayouts.push_back(layout);
    }

    void setSpacing(const int spacing) noexcept
    {
        this->spacing = spacing;
    }

    WindowInterface::IWindow*       window          = nullptr;
protected:

    virtual void caclulateLayout() noexcept = 0;

    std::vector<Widgets::IWidget*>  linkedWidgets;
    std::vector<Layout*>            linkedLayouts;
    int                             spacing         = 5;


};


class VBoxLayout : public Layout
{
public:
    VBoxLayout(WindowInterface::IWindow *window) : Layout(window) {}

	void caclulateLayout() noexcept override
	{
		if (linkedWidgets.empty()) return;

		auto clientRect = applyPaddingToRect(window->getClientRect(), Padding());
		
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
				rect.y = currentY;
				rect.width = clientRect.width;
			}
			else
			{
				rect = { 5, currentY, clientRect.width, flexHeight };
			}

			//rect = applyHeightOnlyPaddingToRect(rect, Padding());
			widget->setRect(rect);
			currentY += rect.height;
		}
	}

};


#endif 