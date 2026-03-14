#include "Direct2dTextFormatCache.hpp"
#include "Renderer/Direct2dTextFormat.hpp"

namespace Renderer
{
    Microsoft::WRL::ComPtr<IDWriteTextFormat>
    Direct2dTextFormatCache::get(const TextFormatStyle& style) const noexcept
    {
        if(cache.contains(style))
        {
            return cache.at(style);
        }
        else
        {
            Direct2dTextFormat direct2dFormat(style.font, {style.alignment, style.paragraphAlignment});
            Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat = direct2dFormat.getTextFormat();
            cache[style] = textFormat;
            return textFormat;
        }
    }

}; 