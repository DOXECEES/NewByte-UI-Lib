#ifndef NBUI_SRC_RENDERER_DIRECT2DTEXTFORMAT_HPP
#define NBUI_SRC_RENDERER_DIRECT2DTEXTFORMAT_HPP

#include <NbCore.hpp>
#include "Direct2dFont.hpp"
#include "TextAlignment.hpp"

namespace Renderer
{


    class Direct2dTextFormat
    {
    public:
        Direct2dTextFormat(const Font& font, TextFormatAlignment alignment) noexcept;
        ~Direct2dTextFormat() noexcept = default;

        NB_COPYMOVABLE(Direct2dTextFormat);
    
        const Microsoft::WRL::ComPtr<IDWriteTextFormat>& getTextFormat() const noexcept;
        
    private:
        Microsoft::WRL::ComPtr<IDWriteTextFormat>	textFormat;

    };
};

#endif