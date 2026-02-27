#pragma once
#include <string>
#include <Vector.hpp>

namespace Localization
{

    struct LocaleId
    {
        std::string language;
        std::string country;
        std::string script;
    };

    struct NumberFormat
    {
        char decimalSeparator = '.';
        char thousandsSeparator = ' ';
        std::string currency;
        char percentSymbol = '%';
        std::string currencySymbol;
    };

    struct DateTimeFormat
    {
        std::string date;
        std::string time;
        std::string dateTime;
    };

    class Locale
    {
    public:
        Locale() noexcept = default;

        // ID локали (language, country, script)
        void setId(const LocaleId& id) noexcept { localeId = id; }
        const LocaleId& id() const noexcept { return localeId; }

        // [names] — человекочитаемые имена
        void setDisplayName(const std::string& name) noexcept { displayName = name; }
        const std::string& getDisplayName() const noexcept { return displayName; }

        void setNativeName(const std::string& name) noexcept { nativeName = name; }
        const std::string& getNativeName() const noexcept { return nativeName; }

        // [number] — формат чисел
        NumberFormat& number() noexcept { return numberFormat; }
        const NumberFormat& number() const noexcept { return numberFormat; }

        // [datetime] — формат даты и времени
        DateTimeFormat& dateTime() noexcept { return dateTimeFormat; }
        const DateTimeFormat& dateTime() const noexcept { return dateTimeFormat; }

        // [meta] — направление текста
        void setTextDirection(const std::string& dir) noexcept { textDirection = dir; }
        const std::string& getTextDirection() const noexcept { return textDirection; }

        // [plural] — правило плюрализации
        void setPluralRule(const std::string& rule) noexcept { pluralRule = rule; }
        const std::string& getPluralRule() const noexcept { return pluralRule; }

        // [fallback] — цепочка локалей
        void setFallbackChain(const nbstl::Vector<std::string>& chain) noexcept { fallbackChain = chain; }
        const nbstl::Vector<std::string>& getFallbackChain() const noexcept { return fallbackChain; }

    private:
        LocaleId localeId;

        std::string displayName;
        std::string nativeName;

        NumberFormat numberFormat;

        DateTimeFormat dateTimeFormat;

        std::string textDirection;

        std::string pluralRule;

        nbstl::Vector<std::string> fallbackChain;
    };

};
