#ifndef NBUI_SRC_RENDERERINTERFACE_IRENDERCONTEXT_HPP
#define NBUI_SRC_RENDERERINTERFACE_IRENDERCONTEXT_HPP

#include <NbCore.hpp>
#include "Core.hpp"

#include "RenderTypes.hpp"

namespace Renderer 
{

    class IResourceManager;

    class IRenderContext 
    {
    public:
        virtual ~IRenderContext() noexcept = default;

        // =============== УПРАВЛЕНИЕ РИСОВАНИЕМ ===============

        // Начать рисование кадра
        virtual void beginDraw() noexcept = 0;

        // Завершить рисование кадра
        // Возвращает: true - успех, false - ошибка (например, потеря устройства)
        [[nodiscard]] virtual bool endDraw() noexcept = 0;

        // Очистить контекст заданным цветом
        virtual void clear(const NbColor& color) noexcept = 0;

        // Принудительно выполнить все отложенные команды
        virtual void flush() noexcept = 0;


        // =============== РИСОВАНИЕ ПРИМИТИВОВ ===============

        // Прямоугольник (контур)
        virtual void drawRectangle(const NbRect<int>& rect, BrushHandle brush) noexcept = 0;

        // Прямоугольник (заливка)
        virtual void fillRectangle(const NbRect<int>& rect, BrushHandle brush) noexcept = 0;

        // Прямоугольник с закругленными углами (контур)
        virtual void drawRoundedRectangle(const NbRect<int>& rect, float radius,
            BrushHandle brush) noexcept = 0;

        // Прямоугольник с закругленными углами (заливка)
        virtual void fillRoundedRectangle(const NbRect<int>& rect, float radius,
            BrushHandle brush) noexcept = 0;

        // Эллипс/окружность (контур)
        virtual void drawEllipse(const NbPoint<int>& center, float radiusX, float radiusY,
            BrushHandle brush) noexcept = 0;

        // Эллипс/окружность (заливка)
        virtual void fillEllipse(const NbPoint<int>& center, float radiusX, float radiusY,
            BrushHandle brush) noexcept = 0;

        // Линия
        virtual void drawLine(const NbPoint<int>& start, const NbPoint<int>& end,
            BrushHandle brush, float strokeWidth = 1.0f) noexcept = 0;


        // =============== ТЕКСТ ===============

        // Нарисовать текст
        virtual void drawText(const std::wstring& text, const NbRect<int>& layoutRect,
            TextFormatHandle format, BrushHandle brush) noexcept = 0;

        // Измерить текст (без рисования)
        [[nodiscard]] virtual NbSize<int> measureText(const std::wstring& text,
            TextFormatHandle format,
            float maxWidth = 0.0f) const noexcept = 0;


        // =============== ИЗОБРАЖЕНИЯ ===============

        // Нарисовать изображение
        virtual void drawBitmap(const NbRect<int>& destination, BitmapHandle bitmap,
            float opacity = 1.0f) noexcept = 0;

        // Нарисовать часть изображения
        virtual void drawBitmap(const NbRect<int>& destination, const NbRect<int>& sourceRect,
            BitmapHandle bitmap, float opacity = 1.0f) noexcept = 0;


        // =============== ГЕОМЕТРИЯ ===============

        // Нарисовать геометрию (контур)
        virtual void drawGeometry(GeometryHandle geometry, BrushHandle brush,
            float strokeWidth = 1.0f) noexcept = 0;

        // Нарисовать геометрию (заливка)
        virtual void fillGeometry(GeometryHandle geometry, BrushHandle brush,
            FillMode fillMode = FillMode::Alternate) noexcept = 0;


        // =============== СОСТОЯНИЕ РИСОВАНИЯ ===============

        // Трансформации (стек)
        //virtual void pushTransform(const Matrix3x2& transform) noexcept = 0;
        //virtual void popTransform() noexcept = 0;
        //[[nodiscard]] virtual Matrix3x2 getCurrentTransform() const noexcept = 0;

        // Область отсечения (стек)
        virtual void pushClip(const NbRect<int>& clipRect) noexcept = 0;
        virtual void popClip() noexcept = 0;

        // Слои (для прозрачности и эффектов)
        virtual void pushLayer(const NbRect<int>& bounds, float opacity = 1.0f) noexcept = 0;
        virtual void popLayer() noexcept = 0;

        // Антиалиасинг
        virtual void setAntialiasMode(AntialiasMode mode) noexcept = 0;
        [[nodiscard]] virtual AntialiasMode getAntialiasMode() const noexcept = 0;


        // =============== ИНФОРМАЦИЯ ===============

        // Получить DPI контекста
        [[nodiscard]] virtual float getDpi() const noexcept = 0;

        // Получить размер render target
        [[nodiscard]] virtual NbSize<int> getSize() const noexcept = 0;

        // Проверить валидность контекста
        [[nodiscard]] virtual bool isValid() const noexcept = 0;


        // =============== СОБЫТИЯ ===============

        // Обработка изменения DPI
        virtual void onDpiChanged(float newDpi) noexcept = 0;

        // Обработка потери графического устройства
        virtual void onDeviceLost() noexcept = 0;


        // =============== СТАТИСТИКА (для отладки) ===============

        struct RenderStats
        {
            uint32_t drawCallCount{ 0 };      // Количество вызовов рисования
            uint32_t primitiveCount{ 0 };     // Количество примитивов
            uint32_t textureCount{ 0 };       // Количество используемых текстур
            uint64_t frameNumber{ 0 };        // Номер кадра
        };

        [[nodiscard]] virtual RenderStats getStats() const noexcept = 0;

        // Сбросить статистику
        virtual void resetStats() noexcept = 0;
    };

}; 

#endif 