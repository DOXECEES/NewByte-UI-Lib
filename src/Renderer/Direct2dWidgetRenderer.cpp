// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#include "Core.hpp"
#include "Direct2dWidgetRenderer.hpp"

#include "Widgets/Button.hpp"
#include "Widgets/TextEdit.hpp"
#include "Widgets/TreeView.hpp"
#include "Widgets/Label.hpp"
#include "Direct2dGlobalWidgetMapper.hpp"

#include "../Widgets/WidgetStyle.hpp"

#include "../Utils.hpp"
#include <Debug.hpp>

#include <stack>

namespace Renderer
{
    Direct2dWidgetRenderer::Direct2dWidgetRenderer(Direct2dHandleRenderTarget *renderTarget)
        :renderTarget(renderTarget)
    {}


    void Direct2dWidgetRenderer::render(IWidget *widget)
    {
        const char *widgetName = widget->getClassName();
        size_t size = strlen(widgetName);

        if(strncmp(widgetName, Button::CLASS_NAME, size) == 0 )
        {
            renderButton(widget);
        }
        else if(strncmp(widgetName, TextEdit::CLASS_NAME, size) == 0 )
        {
            renderTextEdit(widget);
        }
        else if (strncmp(widgetName, TreeView::CLASS_NAME, size) == 0)
        {
            renderTreeView(widget);
        }
        else if(strncmp(widgetName, Label::CLASS_NAME, size) == 0 )
        {
            renderLabel(widget);
        }

    }


    void Direct2dWidgetRenderer::renderButton(IWidget *widget)
    {
        using namespace Widgets;
        Button* button = castWidget<Button>(widget);

        const WidgetStyle& style = button->getStyle();
        WidgetState state = button->getState();
        NbColor color;
        NbColor textColor;

        switch (state)
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
                textColor = style.disableColor;
                break;
            }
            default:
            {
                color = style.baseColor;
                textColor = style.baseTextColor;
                break;
            }
        }

        renderTarget->fillRectangle(button->getRect(), color);
        renderTarget->drawText(button->getText(), button->getRect(), textColor);
    }


    void Direct2dWidgetRenderer::renderTextEdit(IWidget *widget)
    {
        TextEdit* textEdit = castWidget<TextEdit>(widget);
        const NbRect<int> &widgetRect = textEdit->getRect();

        const WidgetStyle& style = textEdit->getStyle();

        renderTarget->drawRectangle(widgetRect, style.baseColor);        

        if(textEdit->getIsDataChanged())
        {
            createTextLayoutForWidget(textEdit);
            textEdit->resetIsDataChanged();
        }

        DWRITE_TEXT_METRICS textMetrics {};

        IDWriteTextLayout *textLayout = Direct2dGlobalWidgetMapper::getTextLayoutByWidget(textEdit);
        textLayout->GetMetrics(&textMetrics);
        HRESULT hr = textLayout->GetMetrics(&textMetrics);
        if (SUCCEEDED(hr))
        {
            float width = textMetrics.widthIncludingTrailingWhitespace;
            float height = textMetrics.height;

            if(width > widgetRect.width)
            {
                int caretPos = textEdit->getCaretPos();

                int halfOfRectWidth = widgetRect.width / 2;

                int startOfString = max(0, caretPos - halfOfRectWidth);
                std::wstring temp = textEdit->getData().substr(startOfString, halfOfRectWidth * 2);

                createTextLayoutForWidget(textEdit, temp);
            }
        }

        renderTarget->drawText(Direct2dGlobalWidgetMapper::getTextLayoutByWidget(textEdit), widgetRect, NbColor(255, 255, 255), TextAlignment::LEFT);
        
        
        if(textEdit->getIsCaretVisible() && textEdit->getIsFocused())
        {
            FLOAT caretX = 0.0f;
            FLOAT caretY = 0.0f;
            DWRITE_HIT_TEST_METRICS hitTestMetrics {};

            HRESULT hr = Direct2dGlobalWidgetMapper::getTextLayoutByWidget(textEdit)->HitTestTextPosition(
                static_cast<UINT32>(textEdit->getCaretPos()),
                FALSE,
                &caretX,
                &caretY,
                &hitTestMetrics
            );

            if (SUCCEEDED(hr))
            {
                D2D1_RECT_F caretRect = D2D1::RectF(
                    widgetRect.x + caretX,
                    widgetRect.y + caretY,
                    widgetRect.x + caretX + 2.0f,
                    widgetRect.y + caretY + hitTestMetrics.height);

                renderTarget->drawRectangle(Utils::toNbRect<int>(caretRect), NbColor(255, 0, 0));
            }
        }
    }

    
    void Direct2dWidgetRenderer::renderTreeView(IWidget* widget)
    {
        TreeView* treeView = castWidget<TreeView>(widget);
        const NbRect<int>& widgetRect = treeView->getRect();

        renderTarget->fillRectangle(widgetRect, treeView->getStyle().baseColor);
        auto model = treeView->getModel();
        if(model == nullptr) return;
        int shift = 0;
        const int shiftX = 10;

        size_t index = 0;
        size_t rootDepth = 128;

        model->forEach([this, &widgetRect, &model, &shift, &index, &treeView, &rootDepth](const ModelItem& item) {
            
            bool isParent = false;

            if (treeView->getItemState(item) == TreeView::ItemState::COLLAPSED)
            {
                rootDepth = item.getDepth();
                isParent = true;
            }
			
            if (rootDepth < item.getDepth() && isParent == false)
            {
				index++;
                return;
            }
            
           
            std::string str = model->data(item);
            std::wstring wstr = Utils::toWstring(str);
            NbRect<int> wr = widgetRect;
            wr.x += shiftX * item.getDepth();
            wr.width -= shiftX * item.getDepth();
            wr.y += shift;
            wr.height = 20;
            shift += 20;

            if (index == treeView->getLastHitIndex())
            {
                renderTarget->drawRectangle({wr.x - int(shiftX * item.getDepth()),
                                             wr.y,
                                             wr.width + int(shiftX * item.getDepth()),
                                             wr.height
                    },
                    { 255,167,133 });
            }

            if (index == treeView->getLastClickedIndex().getRaw())
            {
                renderTarget->fillRectangle({ wr.x - int(shiftX * item.getDepth()),
                                             wr.y,
                                             wr.width + int(shiftX * item.getDepth()),
                                             wr.height
                    },
                    { 255,167,255 });
            }
            
            renderTarget->drawText(wstr, wr, NbColor(255, 255, 255), TextAlignment::LEFT, ParagraphAlignment::TOP);
            index++;
        });
    }

    void Direct2dWidgetRenderer::renderLabel(IWidget *widget)
    {
        Label* label = castWidget<Label>(widget);
        const NbRect<int>& widgetRect = label->getRect();
        renderTarget->drawText(label->getText(), widgetRect, NbColor(255, 255, 255), TextAlignment::LEFT, ParagraphAlignment::TOP);
    }

    void Direct2dWidgetRenderer::createTextLayoutForWidget(IWidget *widget, const std::wstring& data)
    {
        static IDWriteFactory *factory = FactorySingleton::getDirectWriteFactory();
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

            IDWriteTextLayout *textLayout = nullptr;
            IDWriteTextFormat *textFormat = Direct2dGlobalWidgetMapper::getTextFormatByWidget(textEdit);
            if(!textFormat)
            {
                LPCWSTR font = L"Consolas";
                textFormat = Direct2dWrapper::createTextFormatForWidget(textEdit, font);
            }

            const NbRect<int> &rect = widget->getRect();

            factory->CreateTextLayout(newData.c_str(), newData.length(), textFormat, rect.width, rect.height, &textLayout);
            textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            Direct2dGlobalWidgetMapper::addTextlayout(textEdit, textLayout);
        }
        else if (strncmp(widgetName, TreeView::CLASS_NAME, size) == 0)
        {
            TreeView* treeView = castWidget<TreeView>(widget);

            createTextLayoutForTreeView(treeView);
        }
    }


    void Direct2dWidgetRenderer::createTextLayoutForTreeView(TreeView* treeView)
    {
        //DockNode* root = treeView->getTree();

        
    }
    
};