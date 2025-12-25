#ifndef NBUI_SRC_LOCALIZATION_TRANSLATION_HPP
#define NBUI_SRC_LOCALIZATION_TRANSLATION_HPP

#include <string_view>
#include <unordered_map>
#include <string>

namespace Localization
{
	class Translation
	{
	public:
		static void load(std::string_view pathToTranslation) noexcept;
		static const std::string& fromKey(const std::string& key) noexcept;

	private:
		inline static std::unordered_map<std::string, std::string> translationTable;
	};
};

#endif 