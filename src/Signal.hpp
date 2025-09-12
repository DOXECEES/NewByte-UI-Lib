#ifndef NBUI_SRC_SIGNAL_HPP
#define NBUI_SRC_SIGNAL_HPP

template<typename Signature>
class Signal;

template<typename Ret, typename... Args>
class Signal<Ret(Args...)>
{
public:
	using Callback = std::function<Ret(Args...)>;

	void connect(Callback cb) {
		observers.push_back(std::move(cb));
	}

	void emit(Args... args) {
		for (auto& cb : observers)
			cb(args...);
	}

private:
	std::vector<Callback> observers;
};

template<typename T>
struct is_signal : std::false_type {};

template<typename Sig>
struct is_signal<Signal<Sig>> : std::true_type {};

template<typename Publisher, typename SignalType>
constexpr void check_signal(SignalType Publisher::* signal) {
	static_assert(is_signal<SignalType>::value,
		"Field is not Signal<>");

	if (!signal) {
		throw std::invalid_argument("subscribe(): signal is nullptr");
	}
}

// for method
template<typename Publisher, typename SignalType, typename Subscriber, typename Method>
void subscribe(Publisher& publisher,
	SignalType Publisher::* signal,
	Subscriber& subscriber,
	Method method)
{
	check_signal(signal);
	(publisher.*signal).connect([&subscriber, method](auto&&... args) {
		return (subscriber.*method)(std::forward<decltype(args)>(args)...);
		});
}

// for lambda
template<typename Publisher, typename SignalType, typename Func>
void subscribe(Publisher& publisher,
	SignalType Publisher::* signal,
	Func&& func)
{
	check_signal(signal);
	(publisher.*signal).connect(std::forward<Func>(func));
}


#endif