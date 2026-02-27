// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "LocaleManager.hpp"

namespace Localization
{
	void LocaleManager::setCurrent(const Locale& loc) noexcept
	{
		locale = loc;
	}

	const Locale& LocaleManager::current() noexcept
	{
		return locale;
	}

};
