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

#include "Direct2dGlobalWidgetMapper.hpp"

#include "GeometryFactory.hpp"

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
        else if (strncmp(widgetName, CheckBox::CLASS_NAME, size) == 0)
        {
            renderCheckBox(widget);
        }
        else if (strncmp(widgetName, ComboBox::CLASS_NAME, size) == 0)
        {
            renderComboBox(widget);
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


    void Direct2dWidgetRenderer::renderButton(IWidget *widget)
    {
        using namespace Widgets;
        Button* button = castWidget<Button>(widget);

        const WidgetStyle& style = button->getStyle();
        WidgetState state = button->getState();
        NbColor color;
        NbColor textColor;

        getWidgetThemeColorByState(button, color, textColor);


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


        IDWriteTextLayout* textLayout =  Direct2dGlobalWidgetMapper::getTextLayoutByWidget(label);
        
        renderTarget->drawText(textLayout, widgetRect, NbColor(255, 255, 255), static_cast<TextAlignment>(vTextAlign));

    }


	void Direct2dWidgetRenderer::renderCheckBox(IWidget* widget)
	{
		using namespace Widgets;
		CheckBox* checkBox = castWidget<CheckBox>(widget);

		const WidgetStyle& style = checkBox->getStyle();
		WidgetState state = checkBox->getState();
		NbColor color;
		NbColor textColor;

        getWidgetThemeColorByState(checkBox, color, textColor);


        // TODO: cache geometry & add to wrapper class
        // smth like getArrowTextureForCheckBox
        // mb add class figures

        ID2D1PathGeometry* geometry;
        ID2D1GeometrySink* sink;
		HRESULT hr =  FactorySingleton::getFactory()->CreatePathGeometry(&geometry);
         
		if (SUCCEEDED(hr))
		{
			hr = geometry->Open(&sink);

			if (SUCCEEDED(hr))
			{
                const NbRect<int>& boxRc = checkBox->getBoxRect();

                auto vertex = GeometryFactory::create(ShapeType::CHECK_MARK, boxRc);

				sink->BeginFigure(
					D2D1::Point2F(vertex[0].x, vertex[0].y),
					D2D1_FIGURE_BEGIN_FILLED
				);

                for(size_t i = 1; i < vertex.size(); ++i)
                {
					sink->AddLine(D2D1::Point2F(vertex[i].x, vertex[i].y));
                }

                sink->EndFigure(D2D1_FIGURE_END_CLOSED);

				hr = sink->Close();
			}
			SafeRelease(&sink);
		}
        
        auto rt = renderTarget->getRawRenderTarget();
		D2D1_COLOR_F cc = { 1.0f,0.0f,0.0f,1.0f };
        ID2D1SolidColorBrush *c;
		rt->CreateSolidColorBrush(cc, &c);



		renderTarget->fillRectangle(checkBox->getRect(), color);
        renderTarget->drawRectangle(checkBox->getBoxRect(), { 255,255,255 });
        renderLabel(checkBox->getLabel());
        
        
        if(checkBox->getIsChecked()) // some pice of @@
            rt->FillGeometry(geometry, c);

        SafeRelease(&geometry);

	}


	void Direct2dWidgetRenderer::renderComboBox(IWidget* widget)
	{
		using namespace Widgets;
		ComboBox* comboBox = castWidget<ComboBox>(widget);

		const WidgetStyle& style = comboBox->getStyle();
        const NbRect<int>& selectedItemRect = comboBox->getSelectedItemRect();
        WidgetState state = comboBox->getState();
		NbColor color;
		NbColor textColor;
		
        getWidgetThemeColorByState(comboBox, color, textColor);

        renderTarget->fillRectangle(comboBox->getButtonRect(), {128,128,128});
        renderTarget->fillRectangle(comboBox->getSelectedItemRect(), color);

        if (comboBox->getComboState() == ComboBox::ComboState::EXPANDED)
        {
            addWidgetPopUpToQueue(comboBox);
        }
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
		IDWriteTextLayout* textLayout = nullptr;
		Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat = Direct2dGlobalWidgetMapper::getTextFormatByWidget(label);
        if (label->getFont().isDirty())
        {
			textFormat = Direct2dWrapper::createTextFormatForWidget(label, label->getFont());
            label->getFont().clearDirty();
        }

        const NbRect<int>& rect = label->getRect();
        const std::wstring& text = label->getText();

		IDWriteFactory* factory = FactorySingleton::getDirectWriteFactory();
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
		IDWriteFactory* factory = FactorySingleton::getDirectWriteFactory();
		factory->CreateEllipsisTrimmingSign(textFormat.Get(), &inlineEllipsis);

		textFormat->SetTrimming(&trimming, inlineEllipsis.Get());
        

		const NbRect<int>& rect = label->getRect();
		const std::wstring& text = label->getText();

        //std::wstring trimmedText = text.substr(0, lastHitIndex) + L"...";

		factory->CreateTextLayout(text.c_str(), text.length(), textFormat.Get(), rect.width, rect.height, &textLayout);
		textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
		Direct2dGlobalWidgetMapper::addTextlayout(label, textLayout);
	}

};