#include "Direct2dBitmapCache.hpp"
#include "Error/ErrorManager.hpp"


namespace Renderer
{
    Direct2dBitmapCache::Direct2dBitmapCache(
        nbstl::NonOwningPtr<Direct2dHandleRenderTarget> renderTargetLocal
    ) noexcept
        : renderTarget(renderTargetLocal)
    {
        if(!renderTarget)
        {
            nb::Error::ErrorManager::instance()
                .report(nb::Error::Type::FATAL, "RenderTarget is nullptr");
        }
    }



    Microsoft::WRL::ComPtr<ID2D1Bitmap> 
    Direct2dBitmapCache::get(const std::filesystem::path& path) noexcept
    {
        if(cache.contains(path))
        {
            return cache.at(path);
        }

        auto bitmap = renderTarget->loadBitmapFromFile(path.c_str());

        cache[path] = bitmap;
        return bitmap;
    }


};