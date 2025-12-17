#ifndef NBUI_SRC_NEWRENDERER_DIRECT2DFACTORY_HPP
#define NBUI_SRC_NEWRENDERER_DIRECT2DFACTORY_HPP

#include <NbCore.hpp>
#include "Debug.hpp"
#include <d2d1_1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>

#include <string>
namespace direct2dDebugInternal
{
    constexpr DWORD formatMessageFlags =
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS;

    constexpr DWORD dialogFlags = MB_ICONERROR | MB_OK;

    constexpr size_t hresultHexBufferSize = 16;
    constexpr size_t messageReserveSize = 1024;

    constexpr WORD languageId =
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
}

inline void reportDirect2DError(
    HRESULT result,
    const char* expressionText,
    const char* filePath,
    int lineNumber,
    const char* functionName)
{
    using namespace direct2dDebugInternal;

    char* windowsMessageBuffer = nullptr;

    FormatMessageA(
        formatMessageFlags,
        nullptr,
        result,
        languageId,
        reinterpret_cast<LPSTR>(&windowsMessageBuffer),
        0,
        nullptr
    );

    std::string message;
    message.reserve(messageReserveSize);

    message += "[Direct2D ERROR]\n";
    message += "Expression: ";
    message += expressionText;
    message += "\nLocation: ";
    message += filePath;
    message += ":";
    message += std::to_string(lineNumber);
    message += "\nFunction: ";
    message += functionName;

    message += "\nHRESULT: 0x";

    char resultHex[hresultHexBufferSize];
    std::snprintf(
        resultHex,
        sizeof(resultHex),
        "%08X",
        static_cast<unsigned>(result)
    );
    message += resultHex;

    message += "\nMessage: ";
    message += (windowsMessageBuffer ? windowsMessageBuffer : "(no message)");

    OutputDebugStringA(message.c_str());
    MessageBoxA(nullptr, message.c_str(), "Direct2D Error", dialogFlags);

    if (windowsMessageBuffer) {
        LocalFree(windowsMessageBuffer);
    }
    Debug::debug(message);
    //DebugBreak();
}

#define CHECK_DIRECT2D_ERROR(expression) \
do { \
    HRESULT resultCode = (expression); \
    if (FAILED(resultCode)) { \
        reportDirect2DError(resultCode, #expression, __FILE__, __LINE__, __func__); \
    } \
} while (0)

class Direct2DFactory
{
public:
    NB_NON_COPYABLE(Direct2DFactory)

    static Direct2DFactory& getInstance()
    {
        static Direct2DFactory instance;
        return instance;
    }

    ID2D1Factory1* getD2DFactory() const noexcept { return d2dFactory.Get(); }
    IDWriteFactory* getDWriteFactory() const noexcept { return dwriteFactory.Get(); }
    IWICImagingFactory* getWICFactory() const noexcept { return wicFactory.Get(); }

private:
    Microsoft::WRL::ComPtr<ID2D1Factory1> d2dFactory;
    Microsoft::WRL::ComPtr<IDWriteFactory> dwriteFactory;
    Microsoft::WRL::ComPtr<IWICImagingFactory> wicFactory;

    Direct2DFactory()
    {
        D2D1_FACTORY_OPTIONS options{};
#if defined(NB_DEBUG)
        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
        CHECK_DIRECT2D_ERROR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
            options,
            d2dFactory.GetAddressOf())
        );

        CHECK_DIRECT2D_ERROR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(dwriteFactory.GetAddressOf()))
        );

        CHECK_DIRECT2D_ERROR(CoCreateInstance(CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(wicFactory.GetAddressOf()))
        );
    }

};

#endif 
