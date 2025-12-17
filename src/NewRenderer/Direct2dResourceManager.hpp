#ifndef NBUI_SRC_DIRECT2DRESOURCEMANAGER_HPP
#define NBUI_SRC_DIRECT2DRESOURCEMANAGER_HPP

#include "RendererInterface/IResourceManager.hpp"
#include <d2d1_1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <unordered_map>

namespace Renderer
{
    class Direct2DResourceManager : public IResourceManager
    {
    public:
        Direct2DResourceManager(
            ID2D1DeviceContext* deviceContext_,
            IDWriteFactory* writeFactory_,
            IWICImagingFactory* wicFactory_
        )
            : deviceContext(deviceContext_), writeFactory(writeFactory_), wicFactory(wicFactory_)
        {
        }

        // --- Создание ресурсов ---
        BrushHandle createSolidBrush(const NbColor& color) noexcept override
        {
            // Создаем ключ из RGBA
            uint32_t key = (static_cast<uint32_t>(color.a * 255) << 24) |
                (static_cast<uint32_t>(color.r * 255) << 16) |
                (static_cast<uint32_t>(color.g * 255) << 8) |
                (static_cast<uint32_t>(color.b * 255));

            auto it = colorCache.find(key);
            if (it != colorCache.end())
                return it->second;

            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
            D2D1_COLOR_F d2dColor{ color.r, color.g, color.b, color.a };
            HRESULT hr = deviceContext->CreateSolidColorBrush(d2dColor, &brush);
            if (FAILED(hr)) return INVALID_BRUSH_HANDLE;

            BrushHandle handle = registerBrush(brush.Get());
            colorCache[key] = handle;
            return handle;
        }

        TextFormatHandle createTextFormat(const std::wstring& fontFamily, float fontSize) noexcept override
        {
            Microsoft::WRL::ComPtr<IDWriteTextFormat> format;
            HRESULT hr = writeFactory->CreateTextFormat(
                fontFamily.c_str(),
                nullptr,
                DWRITE_FONT_WEIGHT_NORMAL,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                fontSize,
                L"",
                &format
            );
            if (FAILED(hr)) return INVALID_TEXT_FORMAT_HANDLE;

            return registerTextFormat(format.Get());
        }

        BitmapHandle loadBitmap(const std::wstring& filePath) noexcept override
        {
            Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
            HRESULT hr = wicFactory->CreateDecoderFromFilename(
                filePath.c_str(),
                nullptr,
                GENERIC_READ,
                WICDecodeMetadataCacheOnLoad,
                &decoder
            );
            if (FAILED(hr)) return INVALID_BITMAP_HANDLE;

            Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
            hr = decoder->GetFrame(0, &frame);
            if (FAILED(hr)) return INVALID_BITMAP_HANDLE;

            Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap;
            D2D1_BITMAP_PROPERTIES1 props{};
            props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
            props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;

            hr = deviceContext->CreateBitmapFromWicBitmap(frame.Get(), &props, &bitmap);
            if (FAILED(hr)) return INVALID_BITMAP_HANDLE;

            return registerBitmap(bitmap.Get());
        }

        // --- Освобождение ресурсов ---
        void releaseBrush(BrushHandle handle) noexcept override
        {
            brushes.erase(handle);

            // Очистка кэша цветов
            for (auto it = colorCache.begin(); it != colorCache.end(); )
            {
                if (it->second == handle)
                    it = colorCache.erase(it);
                else
                    ++it;
            }
        }

        void releaseTextFormat(TextFormatHandle handle) noexcept override { textFormats.erase(handle); }
        void releaseBitmap(BitmapHandle handle) noexcept override { bitmaps.erase(handle); }

        void clear() noexcept override
        {
            brushes.clear();
            textFormats.clear();
            bitmaps.clear();
            colorCache.clear();
        }

        void onDeviceLost() noexcept override
        {
            brushes.clear();
            bitmaps.clear();
            colorCache.clear();
        }

        // --- Получение нативного COM объекта ---
        void* getNativeBrush(BrushHandle handle) noexcept override
        {
            auto it = brushes.find(handle);
            return it != brushes.end() ? it->second.Get() : nullptr;
        }

        void* getNativeTextFormat(TextFormatHandle handle) noexcept override
        {
            auto it = textFormats.find(handle);
            return it != textFormats.end() ? it->second.Get() : nullptr;
        }

        void* getNativeBitmap(BitmapHandle handle) noexcept override
        {
            auto it = bitmaps.find(handle);
            return it != bitmaps.end() ? it->second.Get() : nullptr;
        }

    protected:
        BrushHandle registerBrush(void* nativeBrush) noexcept override
        {
            static uint32_t idCounter = 1;
            BrushHandle handle{ idCounter++ };
            brushes[handle] = static_cast<ID2D1SolidColorBrush*>(nativeBrush);
            return handle;
        }

        TextFormatHandle registerTextFormat(void* nativeFormat) noexcept override
        {
            static uint32_t idCounter = 1;
            TextFormatHandle handle{ idCounter++ };
            textFormats[handle] = static_cast<IDWriteTextFormat*>(nativeFormat);
            return handle;
        }

        BitmapHandle registerBitmap(void* nativeBitmap) noexcept
        {
            static uint32_t idCounter = 1;
            BitmapHandle handle{ idCounter++ };
            bitmaps[handle] = static_cast<ID2D1Bitmap1*>(nativeBitmap);
            return handle;
        }

    private:
        ID2D1DeviceContext* deviceContext;
        IDWriteFactory* writeFactory;
        IWICImagingFactory* wicFactory;

        std::unordered_map<BrushHandle, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>> brushes;
        std::unordered_map<TextFormatHandle, Microsoft::WRL::ComPtr<IDWriteTextFormat>> textFormats;
        std::unordered_map<BitmapHandle, Microsoft::WRL::ComPtr<ID2D1Bitmap1>> bitmaps;

        // Кэш для кистей по цвету
        std::unordered_map<uint32_t, BrushHandle> colorCache;
    };
}

#endif
