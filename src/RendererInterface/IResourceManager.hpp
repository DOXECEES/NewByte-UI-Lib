#ifndef NBUI_SRC_RENDERERINTERFACE_IRESOURCEMANAGER_HPP
#define NBUI_SRC_RENDERERINTERFACE_IRESOURCEMANAGER_HPP

#include <Core.hpp>

#include "RenderTypes.hpp"

namespace Renderer
{
    class IResourceManager 
    {
    public:
        virtual ~IResourceManager() = default;

        NB_NODISCARD virtual BrushHandle createSolidBrush(
            const NbColor& color) noexcept = 0;

        NB_NODISCARD virtual TextFormatHandle createTextFormat(
            const std::wstring& fontFamily, float fontSize) noexcept = 0;

        NB_NODISCARD virtual BitmapHandle loadBitmap(
            const std::wstring& filePath) noexcept = 0;

        virtual void releaseBrush(BrushHandle handle) noexcept = 0;
        virtual void releaseTextFormat(TextFormatHandle handle) noexcept = 0;
        virtual void releaseBitmap(BitmapHandle handle) noexcept = 0;

        virtual void clear() noexcept = 0;
        virtual void onDeviceLost() noexcept = 0;

        virtual void* getNativeBrush(BrushHandle handle) noexcept = 0;
        virtual void* getNativeTextFormat(TextFormatHandle handle) noexcept = 0;
        virtual void* getNativeBitmap(BitmapHandle handle) noexcept = 0;

    protected:
        virtual BrushHandle registerBrush(void* nativeBrush) noexcept = 0;
        virtual TextFormatHandle registerTextFormat(void* nativeFormat) noexcept = 0;
    };
}; 

#endif
