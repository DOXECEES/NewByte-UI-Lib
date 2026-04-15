#ifndef NBUI_SRC_RENDERER_DIRECT2DWIDGETRENDERER_HPP
#define NBUI_SRC_RENDERER_DIRECT2DWIDGETRENDERER_HPP

#include "Direct2dWrapper.hpp"

#include "Widgets/IWidgetRenderer.hpp"
#include "Widgets/TreeView.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/ComboBox.hpp"
#include "Widgets/Menu.hpp"
#include "Geometry/BorderGeometryCache.hpp"
#include "Geometry/WidgetsGeometryCache.hpp"
#include "Direct2dBitmapCache.hpp"

#include "Direct2dTextFormatCache.hpp"

#include "Debug.hpp"

#include <queue>

namespace Renderer
{
    using namespace Widgets;
    class Direct2dWidgetRenderer : public IWidgetRenderer
    {
    public:
        Direct2dWidgetRenderer(Direct2dHandleRenderTarget* renderTarget);
        void render(IWidget* widget, const NNsLayout::LayoutStyle& layoutStyle) override;
        void renderPopUp() noexcept override;
    
    private:

        struct PopUpRenderParams
        {
            NbRect<int>                 rect;
            std::vector<ListItem>       items;
            NbColor                     color;
            std::vector<NbRect<int>>    addictionalRects;
            std::vector<NbColor>        addictionalColors;
        };

        void drawBorder(IWidget* widget, const Border& border) noexcept;

        void renderButton(IWidget* widget, const NNsLayout::LayoutStyle& layoutStyle);
        void renderTextEdit(IWidget* widget, const NNsLayout::LayoutStyle& layoutStyle);
        void renderTreeView(IWidget* widget, const NNsLayout::LayoutStyle& layoutStyle);
        void renderLabel(IWidget* widget, const NNsLayout::LayoutStyle& layoutStyle);
        void renderCheckBox(IWidget* widget, const NNsLayout::LayoutStyle& layoutStyle);
        void renderComboBox(IWidget* widget, const NNsLayout::LayoutStyle& layoutStyle);
        void renderSpinBox(IWidget* widget, const NNsLayout::LayoutStyle& layoutStyle);
        void renderCalendar(IWidget* widget, const NNsLayout::LayoutStyle& layoutStyle);
        
        void renderSection(
            IWidget* widget,
            const NNsLayout::LayoutStyle& layoutStyle
        );
        void renderColorPicker(
            IWidget* widget,
            const NNsLayout::LayoutStyle& layoutStyle
        );

        void renderToolBar(
            IWidget* widget,
            const NNsLayout::LayoutStyle& layoutStyle
        );

        void renderSlider(
            IWidget* widget,
            const NNsLayout::LayoutStyle& layoutStyle
        );

        void renderThumbnail(
            IWidget* widget,
            const NNsLayout::LayoutStyle& layoutStyle
        );

        void renderMenu(
            IWidget* widget,
            const NNsLayout::LayoutStyle& layoutStyle
        );

        void renderMaterialWidget(
            IWidget*                      widget,
            const NNsLayout::LayoutStyle& layoutStyle
        );

        void renderFilePicker(
            IWidget*                      widget,
            const NNsLayout::LayoutStyle& layoutStyle
        );
        
        void renderTriangleIcon(
            const NbRect<int>& rect,
            bool               expanded,
            const NbColor&     color
        ) noexcept;


        template<typename T>
        T* castWidget(IWidget* widget) const noexcept
        {
            T* castWidget = dynamic_cast<T*>(widget);

            if (!castWidget)
            {
                Debug::debug("The widget name is already registered or the type could not be cast.");
                abort(); // temp 
            }

            return castWidget;
        }

        void getWidgetThemeColorByState(IWidget* widget, NbColor& color, NbColor& textColor) const noexcept;

        void addWidgetPopUpToQueue(IWidget* widget) noexcept
        {
            const char* widgetName = widget->getClassName();
            size_t size = strlen(widgetName);

            if (strncmp(widgetName, ComboBox::CLASS_NAME, size) == 0)
            {
                ComboBox* comboBox = castWidget<ComboBox>(widget);

                size_t dropdownListSize = comboBox->getSize();

                const NbRect<int>& selectedItemRect = comboBox->getSelectedItemRect();
                const NbRect<int>& dropdownRect = comboBox->getDropdownRect();
                
                NbColor color = { 23, 44, 55 };
                std::vector<NbRect<int>> addictionalRect;
                std::vector<NbColor> addictionalColor;

                if (comboBox->getChildrens()[0]->isHover())
                {
                    color = { 23, 44, 55 };
                    addictionalRect.emplace_back(
                        dynamic_cast<DropdownList*>(comboBox->getChildrens()[0].get())->getHoverElementRect()
                    );
                    
                    addictionalColor.emplace_back(128,92,64);
                    
                }

                
                //

                //std::copy()

                PopUpRenderParams params = {
                    dropdownRect,
                    comboBox->getAllItems(),
                    color,
                    addictionalRect,
                    addictionalColor
                };

                popupQueue.push(params);
            }
        }

        void addMenuToPopupQueue(Widgets::Menu* menu);


        void createTextLayoutForWidget(IWidget* widget, const std::wstring& data = L"");

        void createTextLayoutForTreeView(Widgets::TreeView* treeView);
        void createTextLayoutForLabel(Label* label) noexcept;
        void createTextLayoutForLabelClipped(Label* label) noexcept;
        Direct2dHandleRenderTarget* renderTarget = nullptr;


    private:
        std::queue<PopUpRenderParams> popupQueue;
        Geometry::BorderGeometryCache cache;
        Geometry::WidgetsGeometryCache widgetsCache;
        Direct2dBitmapCache bitmapCache;
        

    };


};

#endif