#ifndef NBUI_SRC_WIDGETS_WIDGETSIZEPOLICY_HPP
#define NBUI_SRC_WIDGETS_WIDGETSIZEPOLICY_HPP

namespace Widgets
{
    enum class SizePolicy
    {
        FIXED,          // не растягивается — использует свой rect.width / rect.height
        MINIMUM,        // занимает минимальный размер, который возвращает getRequestedSize()
        PREFERRED,      // использует предпочтительный размер, может немного подрасти
        EXPANDING,      // растягивается, если есть свободное место
        STRETCH         // всегда занимает всё доступное пространство (заполняет ячейку)
    };

    struct WidgetSizePolicy
    {
        SizePolicy horizontal = SizePolicy::PREFERRED;
        SizePolicy vertical = SizePolicy::PREFERRED;

        WidgetSizePolicy() = default;
        WidgetSizePolicy(SizePolicy horiz, SizePolicy vert)
            : horizontal(horiz), vertical(vert) {
        }
    };
}

#endif
