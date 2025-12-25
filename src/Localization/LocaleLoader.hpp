#ifndef NBUI_SRC_LOCALIZATION_LOCALELOADER_HPP
#define NBUI_SRC_LOCALIZATION_LOCALELOADER_HPP

#include <Localization/Locale.hpp>

namespace Localization
{
    class LocalLoader
    {
    public:
        static Locale load(std::string_view pathToLocale) noexcept;
    private:

    };

}; 

#endif 
