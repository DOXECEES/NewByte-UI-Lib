// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "Translation.hpp"
#include <fstream>
#include <filesystem>
#include <Error/ErrorManager.hpp>
#include <StdUtils.hpp>


namespace Localization
{
    void Translation::load(std::string_view pathToTranslation) noexcept
    {
        std::ifstream file(std::filesystem::current_path() / pathToTranslation, std::ios::binary);
        if (!file.is_open())
        {
            nb::Error::ErrorManager::instance()
                .report(nb::Error::Type::FATAL, "Failed to open translation file!")
                .with("Path", pathToTranslation)
                .with("Current Path", std::filesystem::current_path().string());
            return;
        }

        std::stringstream bufferStream;
        bufferStream << file.rdbuf();
        std::string buffer = bufferStream.str();

        if (buffer.size() >= 3 && static_cast<unsigned char>(buffer[0]) == 0xEF &&
            static_cast<unsigned char>(buffer[1]) == 0xBB &&
            static_cast<unsigned char>(buffer[2]) == 0xBF)
        {
            buffer.erase(0, 3);
        }

        size_t pos       = 0;
        size_t lineStart = 0;

        while (pos <= buffer.size())
        {
            if (pos == buffer.size() || buffer[pos] == '\n' || buffer[pos] == '\r')
            {
                std::string line = buffer.substr(lineStart, pos - lineStart);
                lineStart        = pos + 1;
                pos++;

                line = nbstl::trim(line);

                if (line.empty() || line[0] == '#')
                {
                    continue;
                }

                size_t separatorPos = std::string::npos;
                for (size_t i = 0; i < line.size(); ++i)
                {
                    if (line[i] == ':')
                    {
                        bool prevIsColon = (i > 0 && line[i - 1] == ':');
                        bool nextIsColon = (i + 1 < line.size() && line[i + 1] == ':');

                        if (!prevIsColon && !nextIsColon)
                        {
                            separatorPos = i;
                            break;
                        }
                    }
                }

                if (separatorPos != std::string::npos)
                {
                    std::string key   = nbstl::trim(line.substr(0, separatorPos));
                    std::string value = nbstl::trim(line.substr(separatorPos + 1));

                    if (!key.empty())
                    {
                        translationTable[key] = value;
                    }
                }
            }
            else
            {
                pos++;
            }
        }
    }

    const std::string& Translation::fromKey(const std::string& key) noexcept
    {
        if (!translationTable.contains(key))
        {
            nb::Error::ErrorManager::instance()
                .report(nb::Error::Type::INFO, "Key not found in translation table!")
                .with("Key", key);
            return key;
        }


        return translationTable.at(key);
    }

    std::wstring Translation::fromKeyToWstring(const std::string& key) noexcept
    {
        return nbstl::toWString(fromKey(key));
    }
}

