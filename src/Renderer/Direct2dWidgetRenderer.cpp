// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "Core.hpp"
#include "Direct2dWidgetRenderer.hpp"

#include "Widgets/Button.hpp"
#include "Widgets/TextEdit.hpp"
#include "Widgets/TreeView.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/CheckBox.hpp"
#include "Widgets/ComboBox.hpp"
#include "Widgets/SpinBox.hpp"
#include "Widgets/Calendar.hpp"

#include "Direct2dGlobalWidgetMapper.hpp"


#include "../Widgets/WidgetStyle.hpp"

#include "../Utils.hpp"
#include <Debug.hpp>

#include <stack>
#include <Array.hpp>

#include "Geometry/BorderGeometryBuilder.hpp"

namespace Renderer
{
    Direct2dWidgetRenderer::Direct2dWidgetRenderer(Direct2dHandleRenderTarget *renderTarget)
        :renderTarget(renderTarget)
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
        else if (strncmp(widgetName, SpinBox::CLASS_NAME , size) == 0)
        {
            renderSpinBox(widget, layoutStyle);
        }
        else if (strncmp(widgetName, CalendarWidget::CLASS_NAME, size) == 0)
        {
            renderCalendar(widget, layoutStyle);
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
                renderTarget->drawRectangle(rect, border.color, border.width);
                break;
            }
            case Border::Style::DASHED:
            {
                Direct2dHandleRenderTarget::LineStyle ls;
                ls.strokWidth = border.width;
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
                ls.strokWidth = border.width;

                renderTarget->drawStyledLine(rect.getTopLeft(), rect.getTopRight(), border.color, ls);
                renderTarget->drawStyledLine(rect.getBottomLeft(), rect.getBottomRight(), border.color, ls);
                renderTarget->drawStyledLine(rect.getTopLeft(), rect.getBottomLeft(), border.color, ls);
                renderTarget->drawStyledLine(rect.getTopRight(), rect.getBottomRight(), border.color, ls);
                break;
            }
            case Border::Style::DOUBLE:
            {
                renderTarget->drawRectangle(rect.expand(border.width / 2), border.color, border.width);
                NbRect<int> expd = rect.expand(2 * border.width + border.width / 2);
                renderTarget->drawRectangle(expd, border.color, border.width);
                break;
            }
            case Border::Style::INSET:
            {
                auto& mesh = cache.getMesh({ border.style, rect, border.width });
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
                auto& mesh = cache.getMesh({ border.style, rect, border.width });
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
                auto& mesh = cache.getMesh({ border.style, rect, border.width });
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
                auto& mesh = cache.getMesh({ border.style, rect, border.width });
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


    void Direct2dWidgetRenderer::renderButton(IWidget* widget, const NNsLayout::LayoutStyle& layoutStyle)
    {
        using namespace Widgets;
        Button* button = castWidget<Button>(widget);

        const ButtonStyle& bStyle = button->getButtonStyle();
        WidgetState state = button->getState();

        NbColor color, textColor;

        // Определяем цвета в зависимости от состояния
        switch (state) {
        case WidgetState::HOVER:   color = bStyle.hoverColor();   textColor = bStyle.hoverTextColor();   break;
        case WidgetState::ACTIVE:  color = bStyle.activeColor();  textColor = bStyle.activeTextColor();  break;
        case WidgetState::DISABLE: color = bStyle.disableColor(); textColor = bStyle.disableTextColor(); break;
        default:                   color = bStyle.baseColor();    textColor = bStyle.baseTextColor();    break;
        }

        NbRect<int> rect = button->getRect();

        // 1. Рисуем фон кнопки
        renderTarget->fillRectangle(rect, color);

        // 2. Рисуем легкую внутреннюю тень или блик (для объема)
        if (state != WidgetState::ACTIVE && state != WidgetState::DISABLE) {
            NbRect<int> highlight = { rect.x, rect.y, rect.width, 1 };
            renderTarget->fillRectangle(highlight, NbColor(255, 255, 255, 30)); // Белый полупрозрачный блик сверху
        }

        // 3. Рисуем границу (используем ваш drawBorder)
        drawBorder(button, layoutStyle.border);

        // 4. Эффект нажатия: смещаем текст вниз на 1 пиксель, если кнопка активна
        NbRect<int> textRect = rect;
        if (state == WidgetState::ACTIVE)
        {
            textRect.y += 1;
        }

        renderTarget->drawText(button->getText(), textRect, textColor);
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

    
    void Direct2dWidgetRenderer::renderTreeView(IWidget* widget, const NNsLayout::LayoutStyle& layoutStyle)
    {
        using namespace Widgets;

        TreeView* treeView = castWidget<TreeView>(widget);
        const WidgetStyle& style = treeView->getStyle();
        const NbRect<int>& widgetRect = treeView->getRect();
        const TreeViewStyle& treeViewStyle = treeView->getTreeViewStyle();
        renderTarget->fillRectangle(widgetRect, treeView->getStyle().baseColor);

        std::shared_ptr<ITreeModel> model = treeView->getModel();
        if (!model)
        {
            return;
        }

        const int itemHeight = static_cast<int>(TreeView::HEIGHT_OF_ITEM_IN_PIXEL);
        const int indent = 12;

        int y = widgetRect.y;
        size_t visibleCount = treeView->getVisibleCount();

        for (size_t i = 0; i < visibleCount; ++i)
        {
            const ModelItem* item = treeView->getVisibleItem(i);
            if (!item)
            {
                continue;
            }

            NbRect<int> itemRect = {
                widgetRect.x + indent * static_cast<int>(item->getDepth()) + 15,
                y,
                widgetRect.width - indent * static_cast<int>(item->getDepth()) - 15,
                itemHeight
            };

            NbRect<int> itemFullRect = {
                widgetRect.x,
                y,
                widgetRect.width,
                itemHeight
            };


            const nbstl::Uuid uuid = item->getUuid();
            const bool expanded = treeView->isItemExpanded(ModelIndex(uuid));
            const bool selected = treeView->isItemSelected(ModelIndex(uuid));

            
            if (selected)
            {
                renderTarget->fillRectangle(itemFullRect, treeViewStyle.selectionColor);
            }
            else if (treeView->getLastClickIndex().isValid() &&
                treeView->getLastClickIndex().getUuid() == uuid)
            {
                renderTarget->fillRectangle(itemFullRect, treeViewStyle.clickColor);
            }
            else if (treeView->getLastHitIndex().getUuid() == item->getUuid())
            {
                renderTarget->fillRectangle(itemFullRect, treeViewStyle.hoverSelectionColor);
            }

            if (!item->children.empty())
            {
                NbRect<int> box = { itemRect.x - indent + 2, itemRect.y + 6, 8, 8 };
                renderTarget->drawRectangle(box, treeViewStyle.buttonColor);

                if (!expanded)
                {
                    renderTarget->drawLine({ box.x + 4, box.y + 2 }, { box.x + 4, box.y + 6 }, treeViewStyle.inButtonColor); // вертикаль
                }
                renderTarget->drawLine({ box.x + 2, box.y + 4 }, { box.x + 6, box.y + 4 }, treeViewStyle.inButtonColor); // горизонталь
            }

            // Текст
            std::wstring text = Utils::toWstring(model->data(*item));
            renderTarget->drawText(text, itemRect, style.baseTextColor,
                TextAlignment::LEFT, ParagraphAlignment::TOP);

            y += itemHeight;
        }
    }



    void Direct2dWidgetRenderer::renderLabel(IWidget *widget, const NNsLayout::LayoutStyle& layoutStyle)
    {
        Label* label = castWidget<Label>(widget);
        const NbRect<int>& widgetRect = label->getRect();
        const WidgetStyle& style = label->getStyle();

        Label::VTextAlign vTextAlign = label->getVTextAlign();
		Label::HTextAlign hTextAlign = label->getHTextAlign();

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


        Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayout =  Direct2dGlobalWidgetMapper::getTextLayoutByWidget(label);
        
        renderTarget->fillRectangle(widgetRect, style.baseColor);
        renderTarget->drawText(textLayout.Get(), widgetRect, style.baseTextColor, static_cast<TextAlignment>(vTextAlign));
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
        SpinBox* spinBox = castWidget<SpinBox>(widget);
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
		Direct2dGlobalWidgetMapper::addTextlayout(label, textLayout);
	}


	void Direct2dWidgetRenderer::createTextLayoutForLabelClipped(Label* label) noexcept
	{
		IDWriteTextLayout* textLayout = nullptr;
        Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat = Direct2dGlobalWidgetMapper::getTextFormatByWidget(label);
        if (!textFormat || label->getFont().isDirty())
        {
            Direct2dWrapper::createTextFormatForWidget(label, label->getFont());
            textFormat = Direct2dGlobalWidgetMapper::getTextFormatByWidget(label);
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
		Direct2dGlobalWidgetMapper::addTextlayout(label, textLayout);
	}

};//////////////////////////////////////