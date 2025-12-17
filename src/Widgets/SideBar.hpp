#pragma once
#include "Widgets/IWidget.hpp"
#include "Layout.hpp"

#include "Widgets/Label.hpp"

#include <string>

namespace Widgets
{
    struct SidebarState
    {
        std::wstring                tabSelectorTitle;
        NbSize<int>                 minTabSelectorSize;
        NbSize<int>                 maxTabSelectorSize;
        size_t                      tabsCount;
        size_t                      selectedTab;
        std::vector<std::wstring>   tabsName;
    };

    class Sidebar : public IWidget
    {
    public:
        Sidebar(WindowInterface::IWindow* window, const NbRect<int>& rect)
            : IWidget(rect)
            , layout(new FlexLayout(window))
        {
			FlexLayout* lay = new FlexLayout(window, FlexLayout::Direction::Row);
            FlexLayout* lay2 = new FlexLayout(window, FlexLayout::Direction::Row);

            static Button b1({});
            static Button b2({});
            b1.setText(L"b1");
            b2.setText(L"b2");

            static Button b3({});
            static Button b4({});
            b3.setText(L"b3");
            b4.setText(L"b4");

			lay->addWidget(&b1);
            lay->addWidget(&b2);

            lay2->addWidget(&b3);
            lay2->addWidget(&b4);

            //Widgets::Label* label = new Widgets::Label();
            //label->setText(L"Settings");
            //label->setVTextAlign(Widgets::Label::VTextAlign::LEFT);
            //label->setSizePolicy(Widgets::SizePolicy::FIXED ,Widgets::SizePolicy::FIXED);
            //layout->addWidget(label);

            layout->addLayout(lay);
            layout->addLayout(lay2);
        }

        ~Sidebar()
        {
            delete layout;
        }


        void addButton(IWidget* button)
        {
            buttons.push_back(button);
            layout->addWidget(button);
            layout->caclulateLayout(); // пересчЄт автоматически
        }

        

        void toggleCollapse()
        {
            isCollapsed = !isCollapsed;
            recalcLayout();
        }

        virtual bool hitTest(const NbPoint<int>& pos) override
        {
            return rect.isInside(pos);
        }

        virtual const char* getClassName() const override
        {
            return "Sidebar";
        }

    protected:
        void recalcLayout()
        {
            int sidebarWidth = isCollapsed ? 40 : rect.width;
            NbRect<int> sidebarRect = rect;
            sidebarRect.width = sidebarWidth;
            setRect(sidebarRect);

            for (auto* btn : buttons)
            {
                //  нопки используют Layout дл€ расчЄта, поэтому просто вызываем пересчЄт
                layout->caclulateLayout();
            }
        }

    private:
        FlexLayout* layout = nullptr;
        std::vector<IWidget*> buttons;
        bool isCollapsed = false;
        
        SidebarState state;

    };

}
