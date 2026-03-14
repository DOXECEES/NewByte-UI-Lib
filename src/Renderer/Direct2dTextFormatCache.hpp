#ifndef NBUI_SRC_RENDERER_DIRECT2DTEXTFORMATCACHE_HPP
#define NBUI_SRC_RENDERER_DIRECT2DTEXTFORMATCACHE_HPP

#include "Core.hpp"

#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include <wrl/client.h>

#include <unordered_map>


#include "FactorySingleton.hpp"
#include "Direct2dTextFormat.hpp"




namespace Renderer
{    
    struct TextFormatStyle
    {
        Font font;
        TextAlignment alignment = TextAlignment::LEFT;
        ParagraphAlignment paragraphAlignment = ParagraphAlignment::CENTER;
        
        bool operator==(const TextFormatStyle& other) const
        {
            return font == other.font 
                && alignment == other.alignment
                && paragraphAlignment == other.paragraphAlignment;
        }
        
    };

    struct TextFormatStyleHasher {
        size_t operator()(const TextFormatStyle& s) const noexcept {
            size_t seed = 0;
            hash_combine(seed, s.font.getName());
            hash_combine(seed, static_cast<int>(s.font.getStyle()));
            hash_combine(seed, static_cast<int>(s.font.getWeight()));
            hash_combine(seed, static_cast<int>(s.font.getStretch()));
            hash_combine(seed, s.font.getSize());
            hash_combine(seed, static_cast<int>(s.alignment));
            hash_combine(seed, static_cast<int>(s.paragraphAlignment));
            return seed;
        }
    };
    
    class Direct2dTextFormatCache
    {
    public:

        NB_NODISCARD Microsoft::WRL::ComPtr<IDWriteTextFormat> get(const TextFormatStyle& style) const noexcept;        

    private:
        mutable std::unordered_map<TextFormatStyle, Microsoft::WRL::ComPtr<IDWriteTextFormat>, TextFormatStyleHasher> cache;

        Microsoft::WRL::ComPtr<ID2D1Factory1> factory = Renderer::FactorySingleton::getFactory();

    };

};


#endif

