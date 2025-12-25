#include "LocaleLoader.hpp"

#include <String.hpp>

#include <fstream>
#include "Error/ErrorManager.hpp"
#include <StdUtils.hpp>
#include <filesystem>

namespace Localization
{
    Locale LocalLoader::load(std::string_view pathToLocale) noexcept
    {
        std::ifstream file(std::filesystem::current_path().string() + pathToLocale.data(), std::ios::binary);
        if (!file.is_open())
        {
            nb::Error::ErrorManager::instance()
                .report(nb::Error::Type::FATAL, "Failed to open locale file!")
                .with("Path", pathToLocale)
                .with("Current path", std::filesystem::current_path().string());
            return Locale{};
        }

        // Читаем весь файл в буфер
        std::stringstream bufferStream;
        bufferStream << file.rdbuf();
        std::string buffer = bufferStream.str();

        // Убираем BOM если есть
        if (buffer.size() >= 3 &&
            static_cast<unsigned char>(buffer[0]) == 0xEF &&
            static_cast<unsigned char>(buffer[1]) == 0xBB &&
            static_cast<unsigned char>(buffer[2]) == 0xBF)
        {
            buffer.erase(0, 3);
        }

        Locale locale;
        std::string currentSection;
        size_t pos = 0;
        size_t lineStart = 0;

        while (pos <= buffer.size())
        {
            if (pos == buffer.size() || buffer[pos] == '\n')
            {
                std::string line(buffer.begin() + lineStart, buffer.begin() + pos);
                lineStart = pos + 1;
                pos++;

                line = nbstl::trim(line);
                if (line.empty() || line[0] == '#' || line[0] == ';')
                    continue;

                // новая секция
                if (line.front() == '[' && line.back() == ']')
                {
                    currentSection = nbstl::trim(line.substr(1, line.size() - 2));
                    continue;
                }

                auto eqPos = line.find('=');
                if (eqPos == std::string::npos) continue;

                std::string key = nbstl::trim(line.substr(0, eqPos));
                std::string value = nbstl::trim(line.substr(eqPos + 1));

                if (currentSection == "meta")
                {
                    if (key == "code" && value.size() == 5)
                    {
                        LocaleId id{ value.substr(0,2), value.substr(3,2), "" };
                        locale.setId(id);
                    }
                    else if (key == "script") locale.setId({ locale.id().language, locale.id().country, value });
                    else if (key == "direction") locale.setTextDirection(value);
                }
                else if (currentSection == "names")
                {
                    if (key == "display") locale.setDisplayName(value);
                    else if (key == "native") locale.setNativeName(value);
                }
                else if (currentSection == "number")
                {
                    if (key == "decimal" && !value.empty()) locale.number().decimalSeparator = value[0];
                    else if (key == "thousands") locale.number().thousandsSeparator = (value == "space") ? ' ' : value[0];
                    else if (key == "percent" && !value.empty()) locale.number().percentSymbol = value[0];
                    else if (key == "currency") locale.number().currency = value;
                    else if (key == "currencySymbol") locale.number().currencySymbol = value;
                }
                else if (currentSection == "datetime")
                {
                    if (key == "date") locale.dateTime().date = value;
                    else if (key == "time") locale.dateTime().time = value;
                    else if (key == "datetime") locale.dateTime().dateTime = value;
                }
                else if (currentSection == "plural")
                {
                    if (key == "rule") locale.setPluralRule(value);
                }
                else if (currentSection == "fallback")
                {
                    if (key == "chain")
                    {
                        nbstl::Vector<std::string> chain;
                        size_t start = 0;
                        size_t commaPos = 0;
                        while ((commaPos = value.find(',', start)) != std::string::npos)
                        {
                            chain.pushBack(nbstl::trim(value.substr(start, commaPos - start)));
                            start = commaPos + 1;
                        }
                        chain.pushBack(nbstl::trim(value.substr(start)));
                        locale.setFallbackChain(chain);
                    }
                }
            }
            else pos++;
        }

        return locale;
    }

  

};
