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
