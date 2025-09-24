#ifndef NBUI_SRC_RENDERER_DIRECT2DGLOBALWIDGETMAPPER_HPP
#define NBUI_SRC_RENDERER_DIRECT2DGLOBALWIDGETMAPPER_HPP

#include <unordered_map>

#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include <wrl.h>

#include "../Widgets/IWidget.hpp"

namespace Renderer
{

    class Direct2dGlobalWidgetMapper
    {
    public:
        static IDWriteTextLayout* getTextLayoutByWidget(Widgets::IWidget *widget) noexcept;
        static void addTextlayout(Widgets::IWidget* widget, IDWriteTextLayout* textLayout);

        static Microsoft::WRL::ComPtr<IDWriteTextFormat> getTextFormatByWidget(Widgets::IWidget *widget) noexcept;
        static void addTextFormat(Widgets::IWidget* widget, const Microsoft::WRL::ComPtr<IDWriteTextFormat>& textFormat) noexcept;

    private:
        inline static std::unordered_map<int, IDWriteTextLayout *> textLayoutMapper;
        inline static std::unordered_map<int, Microsoft::WRL::ComPtr<IDWriteTextFormat>> textFormatMapper;
    };

};

#endif