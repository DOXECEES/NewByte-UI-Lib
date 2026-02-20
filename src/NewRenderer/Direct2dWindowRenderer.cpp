// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "Direct2dWindowRenderer.hpp"
#include <stdexcept>
#include "Direct2dFactory.hpp"

namespace Renderer
{
    Direct2dWindowRenderer::Direct2dWindowRenderer(WindowInterface::IWindow* window)
    {
        if (!window)
            throw std::runtime_error("Window pointer is null");

        auto& factory = Direct2DFactory::getInstance();
        // Создаём контекст Direct2D для этого окна
        ctx = new Direct2DRenderContext(window->getHandle().as<HWND>(), resourceManager);
        //ctx->initialize();
        resourceManager = new Direct2DResourceManager(
            ctx->getNativeDeviceContext(),
            factory.getDWriteFactory(),
            factory.getWICFactory()
        );

        ctx->setResourceManager(resourceManager);

        // Можно передать фабрики в ResourceManager
       
    }

    Direct2dWindowRenderer::~Direct2dWindowRenderer()
    {
        delete resourceManager;
        delete ctx;
    }

    void Direct2dWindowRenderer::render(WindowInterface::IWindow* window)
    {
        if (!ctx || !window) return;

        const NbSize<int>& windowSize = window->getSize();
        const WindowInterface::WindowStyle& style = window->getStyle();
        
        auto wc = resourceManager->createSolidBrush(window->getColor());
        
        ctx->beginDraw();
        ctx->clear(window->getColor());

        auto frameColor = resourceManager->createSolidBrush( {123,4,55});
        auto fontColor = resourceManager->createSolidBrush(window->getFontColor());
        auto textFormat = resourceManager->createTextFormat(L"Times New Roman", 14);

        const WindowInterface::FrameSize& frameSize = window->getFrameSize();

        const int radius = window->isMaximized() ? 0 : style.getBorderRadius();

        // frame lines without rounded corners //
        NbRect<int> topBorder = { 0, 0, windowSize.width, frameSize.top };
        ctx->fillRectangle(topBorder, frameColor);
        //
        //NbRect<int> botBorder = { 0 + radius, windowSize.height - frameSize.bot, windowSize.width - radius - radius, frameSize.bot };
        //ctx->fillRectangle(botBorder, frameColor);
        //
        //NbRect<int> leftBorder = { 0, frameSize.top, frameSize.left, windowSize.height - frameSize.top - frameSize.bot - radius };
        //ctx->fillRectangle(leftBorder, frameColor);
        //
        //NbRect<int> rightBorder = { windowSize.width - frameSize.right, frameSize.top, frameSize.right, windowSize.height - frameSize.top - frameSize.bot - radius };
        //ctx->fillRectangle(rightBorder, frameColor);
        //

        //NbPoint<int> start = {windowSize.width, windowSize.height - radius};
        //NbPoint<int> end = {windowSize.width - radius, windowSize.height};
        //renderTarget.drawArc(start, end, radius, frameColor);


        //NbRect<int> botRightCorner = {windowSize.width - radius, windowSize.height - radius, radius, radius};
        //renderTarget.fillRectangle(botRightCorner, frameColor);
        //

        if (!frameSize.isEmpty())
            ctx->drawText(window->getTitle(), { 0, 0, windowSize.width, frameSize.top }, textFormat, fontColor);

        //renderTarget.drawText(window->getTitle(), { 0, 0, windowSize.width, windowSize.height }, window->getFontColor());

        //captionButtonRenderer->render(window->getCaptionButtonsContainer());

        // Очистка фона (например, белый)
        //D2D1_COLOR_F clearColor = D2D1::ColorF(D2D1::ColorF::White);
        //ctx->clear({23,45,254});
        //auto c = resourceManager->createSolidBrush({ 223,45,254 });
        //ctx->fillRectangle(window->getClientRect(), c);

        // --- Здесь можно рисовать через ResourceManager ---
        // Пример:
        // auto brush = resourceManager->getNativeBrush(myBrushHandle);
        // d2dContext->DrawRectangle(D2D1::RectF(10,10,100,100), static_cast<ID2D1Brush*>(brush));

        // Завершаем рисование
        CHECK_DIRECT2D_ERROR(
            ctx->endDraw()
        );
    }

    void Direct2dWindowRenderer::resize(WindowInterface::IWindow* window)
    {
        if (!ctx || !window) return;

        // Пересоздаём рендер-таргет для нового размера окна
        ctx->resize(window->getSize().width, window->getSize().height);

        // Device-dependent ресурсы могут зависеть от размера
        resourceManager->onDeviceLost();
    }
}
