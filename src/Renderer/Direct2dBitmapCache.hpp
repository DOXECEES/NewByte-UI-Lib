#ifndef NBUI_SRC_RENDERER_DIRECT2DBITMAPCACHE_HPP
#define NBUI_SRC_RENDERER_DIRECT2DBITMAPCACHE_HPP

#include <NbCore.hpp>

#include <filesystem>
#include <unordered_map>

#include <d2d1.h>
#include <wrl/client.h>

#include "Direct2dWrapper.hpp"
#include "NonOwningPtr.hpp"

namespace Renderer
{
    class Direct2dBitmapCache
    {
    public:
        Direct2dBitmapCache(nbstl::NonOwningPtr<Direct2dHandleRenderTarget> renderTarget) noexcept;
        ~Direct2dBitmapCache() noexcept = default;

        NB_NON_COPYMOVABLE(Direct2dBitmapCache);

        Microsoft::WRL::ComPtr<ID2D1Bitmap> get(const std::filesystem::path& path) noexcept;
        //Microsoft::WRL::ComPtr<ID2D1Bitmap> get(const std::string& path) noexcept;



    private:
        std::unordered_map<std::filesystem::path, Microsoft::WRL::ComPtr<ID2D1Bitmap>> cache;
        nbstl::NonOwningPtr<Direct2dHandleRenderTarget> renderTarget;

    };
};


#endif