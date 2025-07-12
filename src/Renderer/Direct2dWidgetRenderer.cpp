#include "Direct2dWidgetRenderer.hpp"

#include "../Widgets/Button.hpp"
#include "../Widgets/TextEdit.hpp"
#include "Direct2dGlobalWidgetMapper.hpp"

#include "../Utils.hpp"


namespace Renderer
{
    Direct2dWidgetRenderer::Direct2dWidgetRenderer(Direct2dHandleRenderTarget *renderTarget)
        :renderTarget(renderTarget)
    {

    }

    void Direct2dWidgetRenderer::render(IWidget *widget)
    {

        const char *widgetName = widget->getClassName();
        if(strncmp(widgetName, "Button", sizeof(widgetName) / sizeof(char)) == 0 )
        {
            renderButton(widget);
        }
        else if(strncmp(widgetName, "TextEdit", sizeof(widgetName) / sizeof(char)) == 0 )
        {
            renderTextEdit(widget);
        }

    }
    void Direct2dWidgetRenderer::renderButton(IWidget *widget)
    {
        Widgets::Button *button = dynamic_cast<Widgets::Button*>(widget);

        const NbColor &color = button->getIsHover() ? button->getHoverColor() : button->getColor();
        renderTarget->fillRectangle(button->getRect(), color);
        renderTarget->drawText(button->getText(), button->getRect(), NbColor(255, 255, 255));
    }
    void Direct2dWidgetRenderer::renderTextEdit(IWidget *widget)
    {
        Widgets::TextEdit *textEdit = dynamic_cast<Widgets::TextEdit*>(widget);
        const NbRect<int> &widgetRect = textEdit->getRect();

        renderTarget->drawRectangle(widgetRect, textEdit->getColor());
        
        //renderTarget->drawText(textEdit->getData(), textEdit->getRect(), NbColor(255, 255, 255), TextAlignment::LEFT);
        

        if(textEdit->getIsDataChanged())
        {
            createTextLayoutForWidget(textEdit);
            textEdit->resetIsDataChanged();
        }

        DWRITE_TEXT_METRICS textMetrics;

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
            DWRITE_HIT_TEST_METRICS hitTestMetrics;

            HRESULT hr = Direct2dGlobalWidgetMapper::getTextLayoutByWidget(textEdit)->HitTestTextPosition(
                static_cast<UINT32>(textEdit->getCaretPos()),
                FALSE,
                &caretX,
                &caretY,
                &hitTestMetrics);


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
    void Direct2dWidgetRenderer::createTextLayoutForWidget(IWidget *widget, const std::wstring& data)
    {
        static IDWriteFactory *factory = FactorySingleton::getDirectWriteFactory();
        if(strncmp(widget->getClassName(), "TextEdit", sizeof(widget->getClassName()) / sizeof(char)) == 0)
        {


            TextEdit* textEdit = dynamic_cast<Widgets::TextEdit*>(widget);
            std::wstring newData;
            if (data.empty())
            {
                newData = textEdit->getData();
            }
            else
            {
                newData = std::move(data);
            }

            IDWriteTextLayout *textLayout = nullptr;
            IDWriteTextFormat *textFormat = Direct2dGlobalWidgetMapper::getTextFormatByWidget(textEdit);
            if(!textFormat)
            {
                textFormat = Direct2dWrapper::createTextFormatForWidget(textEdit, L"Consolas");
            }

            const NbRect<int> &rect = widget->getRect();

            factory->CreateTextLayout(newData.c_str(), newData.length(), textFormat, rect.width, rect.height, &textLayout);
            textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            Direct2dGlobalWidgetMapper::addTextlayout(textEdit, textLayout);

        }
    }
};