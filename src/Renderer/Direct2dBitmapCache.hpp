#ifndef NBUI_SRC_RENDERER_DIRECT2DBITMAPCACHE_HPP
#define NBUI_SRC_RENDERER_DIRECT2DBITMAPCACHE_HPP

#include <NbCore.hpp>

#include <filesystem>
#include <unordered_map>
#include <future>

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

        struct DecodedData
        {
            std::vector<unsigned char> pixels;
            uint32_t width = 0;
            uint32_t height = 0;
        };

        struct PendingTask
        {
            std::future<DecodedData> task;
        };

        DecodedData loadPixelsFromDisk(std::wstring path)
        {
            CoInitializeEx(NULL, COINIT_MULTITHREADED);

            DecodedData result;
            ComPtr<IWICBitmapDecoder> pDecoder;
            ComPtr<IWICBitmapFrameDecode> pSource;
            ComPtr<IWICFormatConverter> pConverter;
            auto pWICFactory = Renderer::FactorySingleton::getWicFactory();

            if (FAILED(pWICFactory->CreateDecoderFromFilename(
                    path.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder
                )))
            {
                return result;
            }

            pDecoder->GetFrame(0, &pSource);
            pSource->GetSize(&result.width, &result.height);

            pWICFactory->CreateFormatConverter(&pConverter);
            pConverter->Initialize(
                pSource.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f,
                WICBitmapPaletteTypeMedianCut
            );

            result.pixels.resize(result.width * result.height * 4);
            pConverter->CopyPixels(
                NULL, result.width * 4, (UINT)result.pixels.size(), result.pixels.data()
            );

            CoUninitialize(); // Необязательно, если поток умирает сразу
            return result;
        }


    private:
        std::unordered_map<std::filesystem::path, Microsoft::WRL::ComPtr<ID2D1Bitmap>> cache;
        std::unordered_map<std::filesystem::path, PendingTask> pendingTasks;
        nbstl::NonOwningPtr<Direct2dHandleRenderTarget> renderTarget;

    };
};


#endif