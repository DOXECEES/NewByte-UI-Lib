#include "Direct2dGlobalWidgetMapper.hpp"

#include <dwrite.h>

namespace Renderer
{
    IDWriteTextLayout *Direct2dGlobalWidgetMapper::getTextLayoutByWidget(Widgets::IWidget *widget) noexcept
    {
        const int widgetId = widget->getIndex();
        if(textLayoutMapper.find(widgetId) == textLayoutMapper.end())
            return nullptr;
        
        return textLayoutMapper[widgetId];
    }
    void Direct2dGlobalWidgetMapper::addTextlayout(Widgets::IWidget *widget, IDWriteTextLayout *textLayout) 
    {
        const int widgetId = widget->getIndex();
        if(textLayoutMapper.find(widgetId) != textLayoutMapper.end() && textLayoutMapper[widgetId] != nullptr)
        {
            SafeRelease(&textLayoutMapper[widgetId]);
        }
        textLayoutMapper[widgetId] = textLayout;
    }
    IDWriteTextFormat *Direct2dGlobalWidgetMapper::getTextFormatByWidget(Widgets::IWidget *widget) noexcept
    {
        const int widgetId = widget->getIndex();
        if(textFormatMapper.find(widgetId) == textFormatMapper.end())
            return nullptr;
        
        return textFormatMapper[widgetId];
        
    }
    void Direct2dGlobalWidgetMapper::addTextFormat(Widgets::IWidget *widget, IDWriteTextFormat *textFormat) noexcept
    {
        const int widgetId = widget->getIndex();
        if(textFormatMapper.find(widgetId) != textFormatMapper.end() && textFormatMapper[widgetId] != nullptr)
        {
            delete textFormatMapper[widgetId];
        }
        textFormatMapper[widgetId] = textFormat;
    }
};