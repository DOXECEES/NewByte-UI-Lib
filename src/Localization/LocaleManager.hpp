#ifndef NBUI_SRC_LOCALIZATION_LOCALEMANAGER_HPP
#define NBUI_SRC_LOCALIZATION_LOCALEMANAGER_HPP

#include "Localization/Locale.hpp"

namespace Localization 
{
    class LocaleManager
    {
    public:
        static void setCurrent(const Locale& loc) noexcept;
        static const Locale& current() noexcept;

    private:
        inline static Locale locale;
    };
}; 

#endif 
