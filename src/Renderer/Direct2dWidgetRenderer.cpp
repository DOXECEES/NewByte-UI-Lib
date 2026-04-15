// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "Core.hpp"
#include "Direct2dWidgetRenderer.hpp"

#include "Direct2dBitmapCache.hpp"
#include "Renderer/FactorySingleton.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/TextEdit.hpp"
#include "Widgets/TreeView.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/CheckBox.hpp"
#include "Widgets/ComboBox.hpp"
#include "Widgets/SpinBox.hpp"
#include "Widgets/Calendar.hpp"
#include "Widgets/Section.hpp"
#include "Widgets/Slider.hpp"
#include "Widgets/Thumbnail.hpp"
#include "Widgets/Menu.hpp"
#include "Widgets/ToolBar.hpp"
#include "Widgets/MaterialWidget.hpp"
#include "Widgets/FilePicker.hpp"
#include "Direct2dGlobalWidgetMapper.hpp"


#include "../Widgets/WidgetStyle.hpp"
#include "Widgets/ColorPicker.hpp"

#include "../Utils.hpp"
#include <Debug.hpp>

#include <stack>
#include <Array.hpp>
#include <string>

#include "Geometry/BorderGeometryBuilder.hpp"



namespace Renderer
{
    Direct2dWidgetRenderer::Direct2dWidgetRenderer(Direct2dHandleRenderTarget *renderTarget)
        : renderTarget(renderTarget)
        , bitmapCache(renderTarget)
    {}


    void Direct2dWidgetRenderer::render(IWidget *widget, const NNsLayout::LayoutStyle& layoutStyle)
    {
        const char *widgetName = widget->getClassName();
        size_t size = strlen(widgetName);

        if(strncmp(widgetName, Button::CLASS_NAME, size) == 0 )
        {
            renderButton(widget, layoutStyle);
        }
        else if(strncmp(widgetName, TextEdit::CLASS_NAME, size) == 0 )
        {
            renderTextEdit(widget, layoutStyle);
        }
        else if (strncmp(widgetName, TreeView::CLASS_NAME, size) == 0)
        {
            renderTreeView(widget, layoutStyle);
        }
        else if(strncmp(widgetName, Label::CLASS_NAME, size) == 0 )
        {
            renderLabel(widget, layoutStyle);
        }
        else if (strncmp(widgetName, CheckBox::CLASS_NAME, size) == 0)
        {
            renderCheckBox(widget, layoutStyle);
        }
        else if (strncmp(widgetName, ComboBox::CLASS_NAME, size) == 0)
        {
            renderComboBox(widget, layoutStyle);
        }
        else if (strncmp(widgetName, SpinBoxBase::CLASS_NAME , size) == 0)
        {
            renderSpinBox(widget, layoutStyle);
        }
        else if (strncmp(widgetName, CalendarWidget::CLASS_NAME, size) == 0)
        {
            renderCalendar(widget, layoutStyle);
        }
        else if (strncmp(widgetName, SectionWidget::CLASS_NAME, size) == 0)
        {
            renderSection(widget, layoutStyle);
        }
        else if (strncmp(widgetName, ColorPicker::CLASS_NAME, size) == 0)
        {
            renderColorPicker(widget, layoutStyle);
        }
        else if (strncmp(widgetName, ToolBar::CLASS_NAME, size) == 0)
        {
            renderToolBar(widget, layoutStyle);
        }
        else if (strncmp(widgetName, Slider<float>::CLASS_NAME, size) == 0)
        {
            renderSlider(widget, layoutStyle);
        }
        else if (strncmp(widgetName, Thumbnail::CLASS_NAME, size) == 0)
        {
            renderThumbnail(widget, layoutStyle);
        }
        else if (strncmp(widgetName, MaterialWidget::CLASS_NAME, size) == 0)
        {
            renderMaterialWidget(widget, layoutStyle);
        }
        else if (strncmp(widgetName, FilePicker::CLASS_NAME, size) == 0)
        {
            renderFilePicker(widget, layoutStyle);
        }
    }


    void Direct2dWidgetRenderer::renderPopUp() noexcept
    {
        // 00 - 
        // 01 - 
        // 10 - 
        // 11 - 

        // ascending order //

        // store of layers 
        // main             - 0 no overlap
        // default widget   - 1 no overlap
        // popups           - 2 overlap
        // menu | dropdowns - 3 no overlap
        // look for 2 HI-bits
        // other actual z buffer value in category

        while (!popupQueue.empty())
        {
            PopUpRenderParams params = popupQueue.front();
            popupQueue.pop();

            
            renderTarget->fillRectangle(params.rect, params.color);
            if (!params.addictionalRects.empty())
            {
                renderTarget->fillRectangle(params.addictionalRects[0], params.addictionalColors[0]);
            }
            
            size_t index = 0;

            for (const auto& i : params.items)
            {
                NbRect itemRect =  params.rect;
                itemRect.height =  20;
                itemRect.x      += 5;
                itemRect.width  -= 5;
                itemRect.y      =  params.rect.y + (itemRect.height * index);
                renderTarget->drawText(i.getText(), itemRect, {255,255,255}, TextAlignment::LEFT);
                index++;
            }

            
        }
    }

    void Direct2dWidgetRenderer::addMenuToPopupQueue(Widgets::Menu* menu)
    {
        if (!menu)
        {
            return;
        }

        PopUpRenderParams params;

        params.rect = menu->getRect();
        params.color = {30, 30, 30};

        for (const auto& item : menu->getItems())
        {
            params.items.push_back({Utils::toWstring(item.text), item.callback});
        }

        popupQueue.push(params);
    }

    // TODO: изменить width border с top на конкретные размеры
    void Direct2dWidgetRenderer::drawBorder(IWidget* widget, const Border& border) noexcept
    {
        const NbRect<int>& rect = widget->getRect();

        auto drawGeometries = [&](const Geometry::BorderGeometryCache::GeometrySet& mesh, auto& colors)
            {
                for (size_t i = 0; i < mesh.geometries.size(); ++i)
                {
                    if (mesh.geometries[i])
                    {
                        renderTarget->fillGeometry(mesh.geometries[i], colors[i]);
                    }
                }
            };

        switch (border.style)
        {
            case Border::Style::SOLID:
            {
                renderTarget->drawRectangle(rect, border.color, border.width.top);
                break;
            }
            case Border::Style::DASHED:
            {
                Direct2dHandleRenderTarget::LineStyle ls;
                ls.strokWidth = border.width.top;
                ls.dotLength = 3.0f;
                ls.gapLength = 0.5f;

                renderTarget->drawStyledLine(rect.getTopLeft(), rect.getTopRight(), border.color, ls);
                renderTarget->drawStyledLine(rect.getBottomLeft(), rect.getBottomRight(), border.color, ls);
                renderTarget->drawStyledLine(rect.getTopLeft(), rect.getBottomLeft(), border.color, ls);
                renderTarget->drawStyledLine(rect.getTopRight(), rect.getBottomRight(), border.color, ls);
                break;
            }
            case Border::Style::DOTTED:
            {
                Direct2dHandleRenderTarget::LineStyle ls;
                ls.strokWidth = border.width.top;

                renderTarget->drawStyledLine(rect.getTopLeft(), rect.getTopRight(), border.color, ls);
                renderTarget->drawStyledLine(rect.getBottomLeft(), rect.getBottomRight(), border.color, ls);
                renderTarget->drawStyledLine(rect.getTopLeft(), rect.getBottomLeft(), border.color, ls);
                renderTarget->drawStyledLine(rect.getTopRight(), rect.getBottomRight(), border.color, ls);
                break;
            }
            case Border::Style::DOUBLE:
            {
                renderTarget->drawRectangle(rect.expand(border.width.top / 2), border.color, border.width.top);
                NbRect<int> expd = rect.expand(2 * border.width.top + border.width.top / 2);
                renderTarget->drawRectangle(expd, border.color, border.width.top);
                break;
            }
            case Border::Style::INSET:
            {
                auto& mesh = cache.getMesh({border.style, rect, border.width.top});
                NB_ASSERT(mesh.geometries.size() == 2, "INSET border must have exactly 2 geometries");

                nbstl::Array<NbColor, 2> colors = {
                    border.color,
                    border.color.addMask(55)
                };

                drawGeometries(mesh, colors);
                break;
            }
            case Border::Style::OUTSET:
            {
                auto& mesh = cache.getMesh({border.style, rect, border.width.top});
                NB_ASSERT(mesh.geometries.size() == 2, "OUTSET border must have exactly 2 geometries");

                nbstl::Array<NbColor, 2> colors = {
                    border.color.addMask(55),
                    border.color
                };

                drawGeometries(mesh, colors);
                break;
            }
            case Border::Style::RIDGE:
            {
                auto& mesh = cache.getMesh({border.style, rect, border.width.top});
                NB_ASSERT(mesh.geometries.size() == 4, "RIDGE border must have exactly 4 geometries");

                nbstl::Array<NbColor, 4> colors = {
                    border.color.addMask(55),
                    border.color,
                    border.color,
                    border.color.addMask(55)
                };

                drawGeometries(mesh, colors);
                break;
            }
            case Border::Style::GROOVE:
            {
                auto& mesh = cache.getMesh({border.style, rect, border.width.top});
                NB_ASSERT(mesh.geometries.size() == 4, "GROOVE border must have exactly 4 geometries");

                nbstl::Array<NbColor, 4> colors = {
                    border.color,
                    border.color.addMask(55),
                    border.color.addMask(55),
                    border.color
                };

                drawGeometries(mesh, colors);
                break;
            }

        }
    }


   void Direct2dWidgetRenderer::renderButton(
        IWidget* widget,
        const NNsLayout::LayoutStyle& layoutStyle
    )
    {
        using namespace Widgets;
        Button* button = castWidget<Button>(widget);

        const ButtonStyle& bStyle = button->getButtonStyle();
        WidgetState state = button->getState();

        bool isSelected = button->getIsChecked();

        const Font& font = bStyle.font(); 
        const TextFormatAlignment& alignment = bStyle.textAlignment();

        NbColor color, textColor;

        if (state == WidgetState::DISABLE)
        {
            color = bStyle.disableColor();
            textColor = bStyle.disableTextColor();
        }
        else if (state == WidgetState::ACTIVE || isSelected)
        {
            color = bStyle.activeColor();
            textColor = bStyle.activeTextColor();
        }
        else if (state == WidgetState::HOVER)
        {
            color = bStyle.hoverColor();
            textColor = bStyle.hoverTextColor();
        }
        else
        {
            color = bStyle.baseColor();
            textColor = bStyle.baseTextColor();
        }

        NbRect<int> rect = button->getRect();

        renderTarget->fillRectangle(rect, color);

        if (state != WidgetState::DISABLE && state != WidgetState::ACTIVE && !isSelected)
        {
            NbRect<int> highlight = {rect.x, rect.y, rect.width, 1};
            renderTarget->fillRectangle(highlight, NbColor(255, 255, 255, 30));
        }

        drawBorder(button, layoutStyle.border);

        NbRect<int> textRect = rect;

        if (state == WidgetState::ACTIVE || isSelected)
        {
            textRect.y += 1;
        }
        
        Renderer::TextFormatStyle textFormatStyle
        {
            .font = font,
            .alignment = alignment.textAlignment,
            .paragraphAlignment = alignment.paragraphAlignment
        };

        renderTarget->drawTextByFormat(button->getText(), textRect, textColor, textFormatStyle);
    }

    void Direct2dWidgetRenderer::renderSection(
        Widgets::IWidget* widget,
        const NNsLayout::LayoutStyle& layoutStyle
    )
    {
        using namespace Widgets;
        SectionWidget* section = castWidget<SectionWidget>(widget);

        NbRect<int> rect = section->getRect();
        bool isExpanded = section->isExpanded(); // заменяем isExpanded()
        WidgetState state = section->getState();

        constexpr NbColor HEADER_COLOR = {45, 45, 45};
        constexpr NbColor HEADER_HOVER_COLOR = {55, 55, 55};
        constexpr NbColor TEXT_COLOR = {200, 200, 200};
        constexpr NbColor ARROW_COLOR = {150, 150, 150};
        constexpr NbColor SEPARATOR_COLOR = {30, 30, 30, 255};
        constexpr int HEADER_HEIGHT = 24;

        NbColor headerBg = (state == WidgetState::HOVER) ? HEADER_HOVER_COLOR : HEADER_COLOR;
        NbRect<int> headerRect = {rect.x, rect.y, rect.width, HEADER_HEIGHT};

        // Фон шапки
        renderTarget->fillRectangle(headerRect, headerBg);

        // Стрелка раскрытия
        std::wstring arrowSymbol = isExpanded ? L"▼" : L"▶";
        NbRect<int> arrowRect = {headerRect.x + 5, headerRect.y, 20, HEADER_HEIGHT};
        renderTarget->drawText(arrowSymbol.c_str(), arrowRect, ARROW_COLOR);

        // Заголовок
        NbRect<int> titleRect = {
            headerRect.x + 25, headerRect.y, headerRect.width - 25, HEADER_HEIGHT
        };
        renderTarget->drawText(section->getTitle().c_str(), titleRect, TEXT_COLOR);

        // Разделитель
        NbRect<int> separator = {
            headerRect.x, headerRect.y + HEADER_HEIGHT - 1, headerRect.width, 1
        };
        renderTarget->fillRectangle(separator, SEPARATOR_COLOR);

        // Контент
        if (isExpanded && section->getInnerLayout())
        {
            NbRect<int> contentRect = {
                rect.x, rect.y + HEADER_HEIGHT, rect.width, rect.height - HEADER_HEIGHT
            };
            renderTarget->fillRectangle(contentRect, NbColor(35, 35, 35));

            // Рендерим всех дочерних виджетов через innerLayout
            for (auto& childNode : section->getInnerLayout()->getChildren())
            {
                if (!childNode || !childNode->getOwner())
                {
                    continue;
                }

                // безопасный dynamic_cast к IWidget*
                if (auto* childWidget = dynamic_cast<Widgets::IWidget*>(childNode->getOwner()))
                {
                    render(childWidget, layoutStyle);
                }
            }
        }

        // Бордер
        drawBorder(section, layoutStyle.border);
    }

    void Direct2dWidgetRenderer::renderColorPicker(
        IWidget* widget,
        const NNsLayout::LayoutStyle& layoutStyle
    )
    {
        using namespace Widgets;

        ColorPicker* picker = castWidget<ColorPicker>(widget);
        if (!picker)
        {
            return;
        }

        NbColor bgColor, textColor;
        getWidgetThemeColorByState(picker, bgColor, textColor);

        const NbRect<int>& rect = picker->getRect();
        const WidgetStyle& style = picker->getStyle();

        renderTarget->fillRectangle(rect, bgColor);

        // === SV AREA ===
        IWidget* svArea = picker->getSVArea();
        if (svArea)
        {
            const NbRect<int>& svRect = svArea->getRect();
            float hue = picker->getHSV().hue;


            nb::HSV hsv = picker->getHSV();
            nb::Color hueColor = nb::Color::fromHsv(hsv.hue, 1.0f, 1.0f);
            nb::RGB hueRgb = hueColor.toRgb();
            

            renderTarget->fillHorizontalGradient(svRect, NbColor(255, 255, 255), NbColor(hueRgb.r, hueRgb.g, hueRgb.b));

            renderTarget->fillVerticalGradient(svRect, NbColor(0, 0, 0, 0), NbColor(0, 0, 0, 255));

            // === Маркер SV ===
            float s = picker->getHSV().saturation;
            float v = picker->getHSV().value;

            int markerX = svRect.x + int(s * svRect.width);
            int markerY = svRect.y + int((1.0f - v) * svRect.height);

            NbRect<int> marker = {markerX - 4, markerY - 4, 8, 8};
            renderTarget->drawRectangle(marker, NbColor(255, 255, 255));
        }

        // === HUE BAR ===
        IWidget* hueBar = picker->getHueBar();
        if (hueBar)
        {
            const NbRect<int>& hueRect = hueBar->getRect();

            renderTarget->fillHueGradient(hueRect); 

            float h = picker->getHSV().hue;
            int markerY = hueRect.y + int((h / 360.0f) * hueRect.height);

            NbRect<int> marker = {hueRect.x - 2, markerY - 2, hueRect.width + 4, 4};
            renderTarget->fillRectangle(marker, NbColor(255, 255, 255));
        }

        constexpr int MARKER_WIDTH = 6;
        constexpr int MARKER_HEIGHT = 20;

        auto rgbaBars = picker->getRgbaBars();

        for (auto* bar : rgbaBars)
        {
            if (!bar)
            {
                continue;
            }

            const NbRect<int>& rgbaRect = bar->getRect();

            ColorBar::Channel channel = bar->getChannel();

            NbColor leftColor = {0, 0, 0};
            NbColor rightColor;

            switch (channel)
            {
            case ColorBar::Channel::Red:
                rightColor = {255, 0, 0};
                break;

            case ColorBar::Channel::Green:
                rightColor = {0, 255, 0};
                break;

            case ColorBar::Channel::Blue:
                rightColor = {0, 0, 255};
                break;

            case ColorBar::Channel::Alpha:
                rightColor = {255, 255, 255};
                break;
            }

            renderTarget->fillHorizontalGradient(rgbaRect, leftColor, rightColor);

            float t = bar->getValue() / 255.0f;

            int markerX = rgbaRect.x + static_cast<int>(t * rgbaRect.width) - MARKER_WIDTH / 2;

            NbRect<int> marker = {markerX, rgbaRect.y, MARKER_WIDTH, MARKER_HEIGHT};

            renderTarget->fillRectangle(marker, NbColor(255, 255, 255));
        }

        auto spinboxes = picker->getSpinboxes();

        for (auto* spinbox : spinboxes)
        {
            renderSpinBox(spinbox, layoutStyle);
        }

        auto buttons = picker->getButtons();

        for (auto* button : buttons)
        {
            renderButton(button, layoutStyle);
        }

        
        // === PREVIEW ===
        //IWidget* preview = picker->getPreview();
        //if (preview)
        //{
        //    ColorRGB rgb = hsvToRgb(picker->getHSV());

        //    NbColor previewColor(
        //        uint8_t(rgb.r * 255), uint8_t(rgb.g * 255), uint8_t(rgb.b * 255), 255
        //    );

        //    renderTarget->fillRectangle(preview->getRect(), previewColor);
        //    renderTarget->drawRectangle(preview->getRect(), NbColor(60, 60, 60));
        //}

        // === OUTER BORDER ===
        renderTarget->drawRectangle(rect, NbColor(80, 80, 80));
    }

    void Direct2dWidgetRenderer::renderToolBar(
        IWidget* widget,
        const NNsLayout::LayoutStyle& layoutStyle
    )
    {
        using namespace Widgets;
        ToolBar* toolbar = castWidget<ToolBar>(widget);
        const NbRect<int>& widgetRect = toolbar->getRect();
        const WidgetStyle& style = toolbar->getStyle();


        // TODO: border temp
        const Border& border = layoutStyle.border;
        if (border.sideMask == Border::Side::ALL)
        {
            renderTarget->drawRectangle(
                widgetRect, border.color,
                border.width.top
            );
        }
        else
        {
            if (hasFlag(border.sideMask, Border::Side::TOP))
            {
                renderTarget->drawLine(
                    {widgetRect.x, widgetRect.y}, {widgetRect.x + widgetRect.width, widgetRect.y},
                    border.color
                );
            }
            if (hasFlag(border.sideMask, Border::Side::RIGHT))
            {
                renderTarget->drawLine(
                    {widgetRect.x + widgetRect.width, widgetRect.y}, {widgetRect.x + widgetRect.width, widgetRect.y + widgetRect.height},
                    border.color
                );
            }
            if (hasFlag(border.sideMask, Border::Side::LEFT))
            {
                renderTarget->drawLine(
                    {widgetRect.x, widgetRect.y},
                    {widgetRect.x, widgetRect.y + widgetRect.height},
                    border.color
                );
            }
            if (hasFlag(border.sideMask, Border::Side::BOTTOM))
            {
                renderTarget->drawLine(
                    {widgetRect.x, widgetRect.y + widgetRect.height},
                    {widgetRect.x + widgetRect.width, widgetRect.y + widgetRect.height},
                    border.color
                );
            }
        }

        renderTarget->fillRectangle(widgetRect, style.baseColor);

        for (auto& w : toolbar->getChildrens())
        {
            render(w.get(), layoutStyle);
        }

       
    }


    void Direct2dWidgetRenderer::renderTextEdit(IWidget* widget, const NNsLayout::LayoutStyle& layoutStyle)
    {
        using namespace Widgets;
        TextEdit* textEdit = castWidget<TextEdit>(widget);
        const NbRect<int>& widgetRect = textEdit->getRect();
        const WidgetStyle& style = textEdit->getStyle();

        renderTarget->drawRectangle(widgetRect, style.baseColor);

        if (textEdit->getIsDataChanged())
        {
            createTextLayoutForWidget(textEdit);

            auto textLayout = Direct2dGlobalWidgetMapper::getTextLayoutByWidget(textEdit);

            if (textEdit->getIsRTL())
            {
                textLayout->SetReadingDirection(DWRITE_READING_DIRECTION_RIGHT_TO_LEFT);
                textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
            }
            else
            {
                textLayout->SetReadingDirection(DWRITE_READING_DIRECTION_LEFT_TO_RIGHT);
                textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            }

            textEdit->resetIsDataChanged();
        }

        auto textLayout = Direct2dGlobalWidgetMapper::getTextLayoutByWidget(textEdit);

        renderTarget->drawText(textLayout.Get(), widgetRect, NbColor(255, 255, 255), TextAlignment::LEFT);

        if (textEdit->getIsCaretVisible() && textEdit->getIsFocused())
        {
            FLOAT caretX = 0.0f;
            FLOAT caretY = 0.0f;
            DWRITE_HIT_TEST_METRICS hitTestMetrics{};

            HRESULT hr = textLayout->HitTestTextPosition(
                static_cast<UINT32>(textEdit->getCaretPos()),
                FALSE, 
                &caretX,
                &caretY,
                &hitTestMetrics
            );

            if (SUCCEEDED(hr))
            {
                float caretHeight = hitTestMetrics.height;
                caretY = (widgetRect.height - caretHeight) / 2.0f;

                NbRect<float> caretRect = {
                    (float)widgetRect.x + caretX,
                    (float)widgetRect.y + caretY,
                    2.0f, 
                    caretHeight
                };
                
                renderTarget->drawRectangle(caretRect.to<int>(), NbColor(255, 0, 0));
            }
        }
    }


    void Direct2dWidgetRenderer::renderTreeView(
        IWidget*                      widget,
        const NNsLayout::LayoutStyle& layoutStyle
    )
    {
        using namespace Widgets;
        TreeView*            treeView = castWidget<TreeView>(widget);
        const WidgetStyle&   style    = treeView->getStyle();
        const TreeViewStyle& tvStyle  = treeView->getTreeViewStyle();
        const NbRect<int>&   viewRect = treeView->getRect();

        renderTarget->fillRectangle(viewRect, style.baseColor);

        std::shared_ptr<ITreeModel> model = treeView->getModel();
        if (!model)
        {
            return;
        }

        const int itemHeight = static_cast<int>(TreeView::HEIGHT_OF_ITEM_IN_PIXEL);
        const int indentStep = 16;
        const int baseOffset = 12;

        int    y            = viewRect.y;
        size_t visibleCount = treeView->getVisibleCount();

        for (size_t i = 0; i < visibleCount; ++i)
        {
            const ModelItem* item = treeView->getVisibleItem(i);
            if (!item)
            {
                continue;
            }

            const nbstl::Uuid uuid = item->getUuid();
            const ModelIndex  index(uuid);

            bool isSelected    = treeView->isItemSelected(index);
            bool isLastClicked = treeView->getLastClickIndex().isValid() &&
                                 treeView->getLastClickIndex().getUuid() == uuid;

            bool isHovered = (treeView->getLastHitIndex().getUuid() == uuid);

            NbRect<int> fullRowRect = {viewRect.x, y, viewRect.width, itemHeight};

            if (isLastClicked || isSelected)
            {
                renderTarget->fillRectangle(fullRowRect, NbColor{0, 120, 215});
            }
            else if (isHovered)
            {
                renderTarget->fillRectangle(fullRowRect, NbColor{45, 45, 45});
            }

            const int depth    = static_cast<int>(item->getDepth());
            int       contentX = viewRect.x + baseOffset + (depth * indentStep);

            NbColor guideColor = (isSelected || isLastClicked) ? NbColor{60, 140, 230} : NbColor{50, 50, 50};
            for (int d = 0; d < depth; ++d)
            {
                int lineX = viewRect.x + baseOffset + (d * indentStep) - 8;
                renderTarget->drawLine({lineX, y}, {lineX, y + itemHeight}, guideColor);
            }

            if (!item->children.empty())
            {
                bool        isExpanded = treeView->isItemExpanded(index);
                NbRect<int> iconRect   = {contentX - 8, y + (itemHeight - 8) / 2, 8, 8};

                NbColor chevronColor = (isLastClicked || isSelected) ? NbColor{255, 255, 255} : NbColor{140, 140, 140};
                renderTriangleIcon(iconRect, isExpanded, chevronColor);
            }

            NbRect<int> textRect = {
                contentX + 8, y, viewRect.width - (contentX - viewRect.x) - 8, itemHeight
            };

            std::wstring text = treeView->isEditingItem(index)
                                    ? Utils::toWstring(treeView->getEditingText())
                                    : Utils::toWstring(model->data(*item));

            NbColor textColor =
                (isLastClicked || isSelected) ? NbColor{255, 255, 255} : style.baseTextColor;

            renderTarget->drawText(
                text, textRect, textColor, TextAlignment::LEFT, ParagraphAlignment::CENTER
            );

            y += itemHeight;
        }

        if (treeView->hasMenu() && castWidget<Menu>(treeView->getMenu())->isMenuVisible())
        {
            addMenuToPopupQueue(castWidget<Menu>(treeView->getMenu()));
        }
    }

    void Direct2dWidgetRenderer::renderTriangleIcon(
        const NbRect<int>& rect,
        bool               expanded,
        const NbColor&     color
    ) noexcept
    {
        float midX = rect.x + rect.width / 2.0f;
        float midY = rect.y + rect.height / 2.0f;
        float half = rect.width / 2.5f;

        if (expanded)
        {
            renderTarget->drawLine(
                {(int)(midX - half), (int)(midY - half / 2)}, {(int)midX, (int)(midY + half / 2)},
                color
            );
            renderTarget->drawLine(
                {(int)midX, (int)(midY + half / 2)}, {(int)(midX + half), (int)(midY - half / 2)},
                color
            );
        }
        else
        {
            renderTarget->drawLine(
                {(int)(midX - half / 2), (int)(midY - half)}, {(int)(midX + half / 2), (int)midY},
                color
            );
            renderTarget->drawLine(
                {(int)(midX + half / 2), (int)midY}, {(int)(midX - half / 2), (int)(midY + half)},
                color
            );
        }
    }

    void Direct2dWidgetRenderer::renderLabel(
        IWidget*                      widget,
        const NNsLayout::LayoutStyle& layoutStyle
    )
    {
        Label*             label      = castWidget<Label>(widget);
        const NbRect<int>& widgetRect = label->getRect();
        const WidgetStyle& style      = label->getStyle();

        if (style.baseColor.a > 0)
        {
            float radius = layoutStyle.border.radius;
            if (radius > 0.1f)
            {
                renderTarget->fillRoundedRectangle(widgetRect, radius, style.baseColor);
            }
            else
            {
                renderTarget->fillRectangle(widgetRect, style.baseColor);
            }
        }

        if (layoutStyle.border.width.top > 0 || layoutStyle.border.width.bottom > 0)
        {
            float strokeWidth = static_cast<float>(layoutStyle.border.width.bottom);

            if (layoutStyle.border.sideMask == Border::Side::BOTTOM)
            {
                NbRect<int> bottomLine = {
                    static_cast<int>(widgetRect.x),
                    static_cast<int>(widgetRect.y + widgetRect.height - strokeWidth),
                    static_cast<int>(widgetRect.width), static_cast<int>(strokeWidth)
                };
                renderTarget->fillRectangle(bottomLine, layoutStyle.border.color);
            }
            else
            {
                renderTarget->drawRectangle(widgetRect, layoutStyle.border.color, strokeWidth);
            }
        }

        NbRect<int> contentRect = widgetRect;
        contentRect.x += layoutStyle.padding.left;
        contentRect.y += layoutStyle.padding.top;
        contentRect.width -= (layoutStyle.padding.left + layoutStyle.padding.right);
        contentRect.height -= (layoutStyle.padding.top + layoutStyle.padding.bottom);

        if (contentRect.width <= 0 || contentRect.height <= 0)
        {
            return;
        }

        if (label->getFont().isDirty() || label->isSizeChange)
        {
            if (label->hasEllipsis())
            {
                createTextLayoutForLabelClipped(label);
            }
            else
            {
                createTextLayoutForLabel(label);
            }
        }

        Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayout =
            Direct2dGlobalWidgetMapper::getTextLayoutByWidget(label);

        if (textLayout)
        {

            renderTarget->drawText(
                textLayout.Get(), contentRect, style.baseTextColor,
                static_cast<TextAlignment>(label->getVTextAlign())
            );
        }
    }


	void Direct2dWidgetRenderer::renderCheckBox(IWidget* widget, const NNsLayout::LayoutStyle& layoutStyle)
	{
		using namespace Widgets;
		CheckBox* checkBox = castWidget<CheckBox>(widget);

		const WidgetStyle& style = checkBox->getStyle();
		WidgetState state = checkBox->getState();
		NbColor color;
		NbColor textColor;

        getWidgetThemeColorByState(checkBox, color, textColor);
		
		renderTarget->fillRectangle(checkBox->getRect(), color);
        renderTarget->drawRectangle(checkBox->getBoxRect(), { 255,255,255 });
        renderLabel(checkBox->getLabel(), layoutStyle);
        
        if (checkBox->getIsChecked())
        {
            auto geometry = widgetsCache.getMesh(
                Geometry::ShapeType::CHECK_MARK,
                { checkBox->getBoxRect(), checkBox->getIndex() }
            );

            renderTarget->fillGeometry(geometry, { 255,0,0 });

        }
	}

    void Direct2dWidgetRenderer::renderCalendar(IWidget* widget, const NNsLayout::LayoutStyle& layoutStyle)
    {
        using namespace Widgets;
        CalendarWidget* calendar = castWidget<CalendarWidget>(widget);
        if (!calendar) return;

        const auto& grid = calendar->getGrid();
        if (grid.empty()) return;

        // --- Стилизация ---
        const NbColor colBG = { 25, 25, 25, 255 };
        const NbColor colAccent = { 0, 120, 215, 255 };
        const NbColor colText = { 245, 245, 245, 255 };
        const NbColor colMuted = { 100, 100, 100, 255 };
        const NbColor colHover = { 255, 255, 255, 20 };
        const NbColor colBorder = { 45, 45, 45, 255 };

        const NbRect<int>& rect = calendar->getRect();

        // 1. Фон
        renderTarget->fillRectangle(rect, colBG);

        // 2. Шапка
        renderTarget->drawText(calendar->getHeaderText(), calendar->getTitleRect(), colText);
        renderTarget->drawText(L"<", calendar->getBtnPrevRect(), colMuted);
        renderTarget->drawText(L">", calendar->getBtnNextRect(), colMuted);

        // 3. Разделитель
        NbRect<int> sep = { rect.x + 8, calendar->getTitleRect().y + calendar->getTitleRect().height, rect.width - 16, 1 };
        renderTarget->fillRectangle(sep, colBorder);

        // 4. Дни недели (только в режиме DAYS)
        if (calendar->getViewMode() == CalendarViewMode::DAYS) {
            int cellW = rect.width / calendar->getWeekLength();
            int dowY = sep.y + 5;
            for (int i = 0; i < calendar->getWeekLength(); ++i) {
                NbRect<int> dowRect = { rect.x + (i * cellW), dowY, cellW, 20 };
                renderTarget->drawText(calendar->getDayName(i), dowRect, colMuted);
            }
        }

        // 5. Сетка данных
        for (const auto& cell : grid) {
            // "Внутренняя" область ячейки для визуальных эффектов
            NbRect<int> vRect = cell.rect;
            vRect.x += 2; vRect.y += 2; vRect.width -= 4; vRect.height -= 4;

            if (cell.isSelected) {
                renderTarget->fillRectangle(vRect, colAccent);
            }
            else if (cell.isHovered && cell.isCurrentRange) {
                renderTarget->fillRectangle(vRect, colHover);
            }

            NbColor curColor = colText;
            if (!cell.isCurrentRange) curColor = colMuted;
            if (cell.isSelected)      curColor = { 255, 255, 255, 255 };

            renderTarget->drawText(cell.text, cell.rect, curColor);

            // Индикатор "Сегодня"
            if (cell.isToday) {
                if (cell.isSelected) {
                    renderTarget->drawRectangle(vRect, { 255, 255, 255, 180 });
                }
                else {
                    NbRect<int> indicator = { cell.rect.x + cell.rect.width / 2 - 7, cell.rect.y + cell.rect.height - 5, 14, 2 };
                    renderTarget->fillRectangle(indicator, colAccent);
                }
            }
        }
    }

    void Direct2dWidgetRenderer::renderComboBox(IWidget* widget, const NNsLayout::LayoutStyle& layoutStyle)
    {
        using namespace Widgets;
        ComboBox* comboBox = castWidget<ComboBox>(widget);

        const NbRect<int>& rect = comboBox->getRect();
        const NbRect<int>& buttonRect = comboBox->getButtonRect();
        const NbRect<int>& selectedItemRect = comboBox->getSelectedItemRect();

        NbColor bgColor, textColor;
        getWidgetThemeColorByState(comboBox, bgColor, textColor);

        // 1. Фон
        renderTarget->fillRectangle(rect, bgColor);

        // 2. Текст
        NbRect<int> textRect = selectedItemRect;
        textRect.x += 8;
        textRect.width -= 8;
        renderTarget->drawText(comboBox->getSelectedItem().getText(), textRect, textColor);

        // 3. Цвет кнопки (Исправляем narrowing conversion для uint8_t)
        auto adjust = [](uint8_t val) -> uint8_t {
            return static_cast<uint8_t>(val > 20 ? val - 20 : val + 20);
            };
        NbColor buttonColor = { adjust(bgColor.r), adjust(bgColor.g), adjust(bgColor.b) };

        renderTarget->fillRectangle(buttonRect, buttonColor);

        // 4. Стрелочка (Исправляем narrowing conversion из float в int для точек)
        float centerX = static_cast<float>(buttonRect.x) + static_cast<float>(buttonRect.width) / 2.0f;
        float centerY = static_cast<float>(buttonRect.y) + static_cast<float>(buttonRect.height) / 2.0f;
        float size = 4.0f;

       
        if (comboBox->getComboState() == ComboBox::ComboState::EXPANDED)
        {
            renderTarget->drawLine(
                nbui::makePoint<int>(centerX - size, centerY + size / 2.0f),
                nbui::makePoint<int>(centerX, centerY - size / 2.0f),
                textColor
            );
            renderTarget->drawLine(
                nbui::makePoint<int>(centerX, centerY - size / 2.0f),
                nbui::makePoint<int>(centerX + size, centerY + size / 2.0f),
                textColor
            );

            addWidgetPopUpToQueue(comboBox);
        }
        else
        {
            renderTarget->drawLine(
                nbui::makePoint<int>(centerX - size, centerY - size / 2.0f),
                nbui::makePoint<int>(centerX, centerY + size / 2.0f),
                textColor
            );
            renderTarget->drawLine(
                nbui::makePoint<int>(centerX, centerY + size / 2.0f),
                nbui::makePoint<int>(centerX + size, centerY - size / 2.0f), 
                textColor
            );
        }

        renderTarget->drawRectangle(rect, textColor, 1.0f);
    }

    void Direct2dWidgetRenderer::renderSpinBox(IWidget* widget, const NNsLayout::LayoutStyle& layoutStyle)
    {
        using namespace Widgets;
        SpinBoxBase* spinBox = castWidget<SpinBoxBase>(widget);
        if (!spinBox) return;

        NbColor bgColor, textColor;
        getWidgetThemeColorByState(spinBox, bgColor, textColor);

        const NbRect<int>& rect = spinBox->getRect();
        const WidgetStyle& style = spinBox->getStyle();

        renderTarget->fillRectangle(rect, bgColor);

        if (spinBox->getInput())
        {
            renderTextEdit(spinBox->getInput(), layoutStyle);
        }

        if (spinBox->getUpButton()) 
        {
            renderButton(spinBox->getUpButton(), layoutStyle);
        }

        if (spinBox->getDownButton()) 
        {
            renderButton(spinBox->getDownButton(), layoutStyle);
        }


        if (spinBox->getUpButton()) 
        {
            int separatorX = spinBox->getUpButton()->getRect().x;
            NbRect<int> separator = { separatorX - 1, rect.y + 2, 1, rect.height - 4 };
            renderTarget->fillRectangle(separator, NbColor(100, 100, 100, 100)); 
        }

            renderTarget->drawRectangle(rect, NbColor(80, 80, 80));
        //if (spinBox->getInput() && spinBox->getInput()->getIsFocused()) {
        //    // Рисуем внешнюю рамку толщиной 2 пикселя (акцентную)
        //    renderTarget->drawRectangle(rect, NbColor(0, 120, 215));
        //}
        //else {
        //    // Обычная рамка
        //}
    }

    void Direct2dWidgetRenderer::renderSlider(IWidget* widget, const NNsLayout::LayoutStyle& layoutStyle)
    {
        using namespace Widgets;
        Slider<float>* slider = castWidget<Slider<float>>(widget);

        const NbRect<int>& rect = slider->getRect();
        const WidgetStyle& style = slider->getStyle();

        NbColor backgroundColor;
        NbColor textColor;

        getWidgetThemeColorByState(slider, backgroundColor, textColor);

        renderTarget->fillRectangle(rect, backgroundColor);
        
        renderTarget->drawText(std::to_wstring(slider->getValue()), rect, textColor);
    }

    void Direct2dWidgetRenderer::renderThumbnail(IWidget* widget, const NNsLayout::LayoutStyle& layoutStyle)
    {
        using namespace Widgets;
        Thumbnail* thumbnail = castWidget<Thumbnail>(widget);

        const NbRect<int>& rect = thumbnail->getRect();
        NbColor backgroundColor;
        NbColor textColor;

        getWidgetThemeColorByState(thumbnail, backgroundColor, textColor);
        
        NbRect<int> cardRect = {
            rect.x - 2, rect.y - 2, rect.width + 4, rect.height + 4

        };

        renderTarget->fillRoundedRectangle(cardRect, 4, backgroundColor);

        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
        

        NbRect<int> imgRect = thumbnail->getDrawRect();
        imgRect.x += 2;
        imgRect.y += 2;
        imgRect.width -= 4;
        imgRect.height -= 4;

        auto bitmap = bitmapCache.get(
            std::wstring(
                L"C:\\Repos\\Engine\\NewByte-Engine\\out\\build\\x64-Debug\\SDK\\Assets\\res\\"
            ) +
            thumbnail->getName()
        );
        if (bitmap)
        {
            renderTarget->drawBitmap(imgRect, bitmap.Get());
        }
    }

    void Direct2dWidgetRenderer::renderMenu(
        IWidget* widget,
        const NNsLayout::LayoutStyle& layoutStyle
    )
    {
        using namespace Widgets;
        Menu* menu = castWidget<Menu>(widget);

        const NbRect<int>& rect = menu->getRect();
        NbColor backgroundColor;
        NbColor textColor;

        getWidgetThemeColorByState(menu, backgroundColor, textColor);

        renderTarget->fillRectangle(rect, backgroundColor);
        
        for (const auto& item : menu->getItems())
        {
            renderTarget->drawText(Utils::toWstring(item.text), item.rect, textColor);
        }
    }

    void Direct2dWidgetRenderer::renderMaterialWidget(
        IWidget*                      widget,
        const NNsLayout::LayoutStyle& layoutStyle
    )
    {
        using namespace Widgets;
        MaterialWidget* matWidget = castWidget<MaterialWidget>(widget);
        if (!matWidget)
        {
            return;
        }

        const NbRect<int>& rect = matWidget->getRect();
        NbColor            bgColor, textColor;
        getWidgetThemeColorByState(matWidget, bgColor, textColor);

        NbColor slotBg = {35, 35, 35, 255};
        if (matWidget->getState() == Widgets::WidgetState::HOVER)
        {
            slotBg = {45, 45, 45, 255}; 
        }

        renderTarget->fillRoundedRectangle(rect, 2, slotBg);

        NbRect<int> pRect = matWidget->getPreviewRect();
        NbColor     matColor =
            matWidget->isMaterialAssigned() ? NbColor(70, 110, 190) : NbColor(50, 50, 50);

        renderTarget->fillRectangle(pRect, matColor);
        renderTarget->drawRectangle(pRect, {0, 0, 0, 100}, 1.0f);

        if (matWidget->getNameLabel())
        {
            renderLabel(matWidget->getNameLabel().get(), layoutStyle);
        }
        if (matWidget->getTypeLabel())
        {
            renderLabel(matWidget->getTypeLabel().get(), layoutStyle);
        }

        NbColor borderColor = matWidget->getState() == Widgets::WidgetState::HOVER
                                  ? NbColor(0, 120, 215)
                                  : NbColor(60, 60, 60);
        renderTarget->drawRectangle(rect, borderColor, 1.0f);
    }

    void Direct2dWidgetRenderer::renderFilePicker(
        IWidget*                      widget,
        const NNsLayout::LayoutStyle& layoutStyle
    )
    {
        using namespace Widgets;

        FilePicker* picker = castWidget<FilePicker>(widget);
        if (!picker)
        {
            return;
        }

        // 1. Общий фон окна
        NbColor bgColor, textColor;
        getWidgetThemeColorByState(picker, bgColor, textColor);
        const NbRect<int>& rect = picker->getRect();

        renderTarget->fillRectangle(rect, bgColor); // Основной фон

        // 2. Рендерим текстовые поля (Путь и Имя файла)
        // Если у вас есть renderTextBox, используем его.
        // Если нет, текстовые поля часто рендерятся как SpinBox без кнопок.
        renderTextEdit(picker->getPathTextBox().get(), layoutStyle);
        renderTextEdit(picker->getFileNameTextBox().get(), layoutStyle);

        // 3. Рендерим кнопки (Up, Open, Cancel)
        renderButton(picker->getUpButton().get(), layoutStyle);
        renderButton(picker->getOpenButton().get(), layoutStyle);
        renderButton(picker->getCancelButton().get(), layoutStyle);

        // 4. Рендерим ListView (Самая важная часть)
        ListView* listView = picker->getFileListView().get();
        if (listView)
        {
            const NbRect<int>& lvRect = listView->getRect();

            // Фон списка (обычно чуть темнее или светлее основного)
            renderTarget->fillRectangle(lvRect, NbColor(30, 30, 30));
            renderTarget->drawRectangle(lvRect, NbColor(60, 60, 60)); // Рамка списка

            // Отрисовка элементов списка
            size_t count       = listView->getCount();
            int    itemHeight  = ListView::HEIGHT_OF_ITEM_IN_PIXEL;
            auto   selectedIdx = listView->getSelectedIndex();

            // Настраиваем отсечение (Clip), чтобы текст не вылезал за границы ListView
            renderTarget->pushClip(lvRect);

            for (size_t i = 0; i < count; ++i)
            {
                // Вычисляем позицию Y с учетом прокрутки (range.first)
                // Примечание: предполагается, что в ListView есть доступ к range
                int itemY = lvRect.y + (int(i) * itemHeight) - listView->getScrollOffset();

                // Пропускаем те, что вне видимости
                if (itemY + itemHeight < lvRect.y || itemY > lvRect.y + lvRect.height)
                {
                    continue;
                }

                NbRect<int> itemRect = {lvRect.x, itemY, lvRect.width, itemHeight};

                // Подсветка выбранного элемента
                if (selectedIdx.has_value() && selectedIdx.value() == i)
                {
                    renderTarget->fillRectangle(itemRect, NbColor(60, 120, 180, 150));
                }
                // Подсветка при наведении (если в ListView хранится lastHitIndex)
                else if (listView->getLastHitIndex() == (int)i)
                {
                    renderTarget->fillRectangle(itemRect, NbColor(255, 255, 255, 30));
                }

                // Отрисовка текста элемента
                std::string text = listView->getItemText(i);
                // Используем стандартный метод отрисовки текста вашего движка
                NbRect<int> textRect = {
                    itemRect.x + 5, itemRect.y, itemRect.width - 10, itemRect.height
                };
                renderTarget->drawText(
                    Utils::toWstring(text), textRect, NbColor(220, 220, 220)
                );
            }

            renderTarget->popClip();
        }

        // 5. Внешняя рамка всего окна
        renderTarget->drawRectangle(rect, NbColor(80, 80, 80));
    }


    void Direct2dWidgetRenderer::getWidgetThemeColorByState(IWidget* widget, NbColor& color, NbColor& textColor) const noexcept
    {
        const WidgetStyle& style = widget->getStyle();

        switch (widget->getState())
        {
            case WidgetState::HOVER:
            {
                color = style.hoverColor;
                textColor = style.hoverTextColor;
                break;
            }
                case WidgetState::ACTIVE:
            {
                color = style.activeColor;
                textColor = style.activeTextColor;
                break;
            }
            case WidgetState::DISABLE:
            {
                color = style.disableColor;
                textColor = style.disableTextColor;
                break;
            }
            default:
            {
                color = style.baseColor;
                textColor = style.baseTextColor;
                break;
            }
        }
    }


    void Direct2dWidgetRenderer::createTextLayoutForWidget(IWidget* widget, const std::wstring& data)
    {
        static Microsoft::WRL::ComPtr<IDWriteFactory> factory = FactorySingleton::getDirectWriteFactory();
        const char* widgetName = widget->getClassName();
        size_t size = strlen(widgetName);

        if(strncmp(widgetName, TextEdit::CLASS_NAME, size) == 0)
        {
            TextEdit* textEdit = castWidget<TextEdit>(widget);

            std::wstring newData;
            if (data.empty())
            {
                newData = textEdit->getData();
            }
            else
            {
                newData = data;
            }

            Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayout = nullptr;
            Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat = Direct2dGlobalWidgetMapper::getTextFormatByWidget(textEdit);
            
            if(!textFormat)
            {
                Font font;
                textFormat = Direct2dWrapper::createTextFormatForWidget(textEdit, font);
            }

            const NbRect<int> &rect = widget->getRect();

            factory->CreateTextLayout(newData.c_str(), newData.length(), textFormat.Get(), rect.width, rect.height, &textLayout);
            textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            Direct2dGlobalWidgetMapper::addTextlayout(textEdit, textLayout);
        }
        else if (strncmp(widgetName, TreeView::CLASS_NAME, size) == 0)
        {
            TreeView* treeView = castWidget<TreeView>(widget);

            createTextLayoutForTreeView(treeView);
        }
        else if (strncmp(widgetName, Label::CLASS_NAME, size) == 0)
        {
            Label* label = castWidget<Label>(widget);
            createTextLayoutForLabel(label);
        }
    }


    void Direct2dWidgetRenderer::createTextLayoutForTreeView(TreeView* treeView)
    {
        //DockNode* root = treeView->getTree();

        
    }

    
	void Direct2dWidgetRenderer::createTextLayoutForLabel(Label* label) noexcept
	{
        Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayout = nullptr;
		Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat = Direct2dGlobalWidgetMapper::getTextFormatByWidget(label);
        if (label->getFont().isDirty())
        {
			textFormat = Direct2dWrapper::createTextFormatForWidget(label, label->getFont());
            label->getFont().clearDirty();
        }

        const NbRect<int>& rect = label->getRect();
        const std::wstring& text = label->getText();

        Microsoft::WRL::ComPtr<IDWriteFactory> factory = FactorySingleton::getDirectWriteFactory();
		factory->CreateTextLayout(text.c_str(), text.length(), textFormat.Get(), rect.width, rect.height, &textLayout);
		textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
		Direct2dGlobalWidgetMapper::addTextlayout(label, std::move(textLayout));
	}


	void Direct2dWidgetRenderer::createTextLayoutForLabelClipped(Label* label) noexcept
	{
        Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayout = nullptr;
        Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat = Direct2dGlobalWidgetMapper::getTextFormatByWidget(label);
        if (!textFormat || label->getFont().isDirty())
        {
            textFormat = Direct2dWrapper::createTextFormatForWidget(label, label->getFont());
            //Direct2dGlobalWidgetMapper::getTextFormatByWidget(label);
            label->getFont().clearDirty();
        }

		DWRITE_TRIMMING trimming = {};
		trimming.granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;
		trimming.delimiter = 0;
		trimming.delimiterCount = 0;

		Microsoft::WRL::ComPtr<IDWriteInlineObject> inlineEllipsis;
        Microsoft::WRL::ComPtr<IDWriteFactory> factory = FactorySingleton::getDirectWriteFactory();
		factory->CreateEllipsisTrimmingSign(textFormat.Get(), &inlineEllipsis);

		textFormat->SetTrimming(&trimming, inlineEllipsis.Get());
        

		const NbRect<int>& rect = label->getRect();
		const std::wstring& text = label->getText();

        //std::wstring trimmedText = text.substr(0, lastHitIndex) + L"...";

		factory->CreateTextLayout(text.c_str(), text.length(), textFormat.Get(), rect.width, rect.height, &textLayout);
		textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
		Direct2dGlobalWidgetMapper::addTextlayout(label, std::move(textLayout));
	}

};//////////////////////////////////////