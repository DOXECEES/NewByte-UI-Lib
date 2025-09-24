#ifndef NBUI_SRC_SUBSCRIPTIONMANAGER_HPP
#define NBUI_SRC_SUBSCRIPTIONMANAGER_HPP

//#include "Signal.hpp"
#include "Widgets/IWidget.hpp"	

#include <unordered_set>

class SubscriptionManager
{
public:

	bool isWidgetSubscribedForRenderer(const Widgets::IWidget* widget) const noexcept;

private:
	std::unordered_set<int> subscribedWidgetIds;
};

#endif 