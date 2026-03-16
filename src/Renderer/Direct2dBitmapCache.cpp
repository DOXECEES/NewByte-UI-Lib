#include "Direct2dBitmapCache.hpp"
#include "Error/ErrorManager.hpp"
#include <future>

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
        if (cache.contains(path))
        {
            return cache.at(path);
        }

        if (pendingTasks.contains(path))
        {
            auto& pending = pendingTasks[path];
            if (pending.task.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {

                DecodedData data = pending.task.get(); 
                pendingTasks.erase(path);

                if (!data.pixels.empty())
                {
                    Microsoft::WRL::ComPtr<ID2D1Bitmap> bitmap;
                    auto props = D2D1::BitmapProperties(
                        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
                    );

                    renderTarget->getRawContext()->CreateBitmap(
                        D2D1::SizeU(data.width, data.height), data.pixels.data(), data.width * 4,
                        &props, &bitmap
                    );

                    cache[path] = bitmap;
                    return bitmap;
                }
            }
            return nullptr;
        }

        pendingTasks[path].task = std::async(
            std::launch::async, &Direct2dBitmapCache::loadPixelsFromDisk, this, path.wstring()
        );

        return nullptr;
    }


};