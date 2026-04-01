#ifndef NBUI_SRC_SIGNAL_HPP
#define NBUI_SRC_SIGNAL_HPP

#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>
#include <vector>


    /**
     * @brief Класс соединения. Позволяет разорвать связь между сигналом и слотом.
     */
    class Connection
    {
    public:
        Connection() = default;
        Connection(std::function<void()> disconnect_fn) : m_remover(std::move(disconnect_fn))
        {
        }

        // Разрывает связь вручную
        void disconnect()
        {
            if (m_remover)
            {
                m_remover();
                m_remover = nullptr;
            }
        }

        bool isConnected() const
        {
            return m_remover != nullptr;
        }

    private:
        std::function<void()> m_remover;
    };

    /**
     * @brief RAII-обертка для соединения. Разрывает связь автоматически при выходе из области
     * видимости.
     */
    class ScopedConnection
    {
    public:
        ScopedConnection(Connection c) : m_conn(std::move(c))
        {
        }
        ~ScopedConnection()
        {
            m_conn.disconnect();
        }

        ScopedConnection(const ScopedConnection&) = delete;
        ScopedConnection& operator=(const ScopedConnection&) = delete;
        ScopedConnection(ScopedConnection&&) noexcept = default;
        ScopedConnection& operator=(ScopedConnection&&) noexcept = default;

    private:
        Connection m_conn;
    };

    template <typename Signature>
    class Signal;

    template <typename Ret, typename... Args>
    class Signal<Ret(Args...)>
    {
    public:
        using Callback = std::function<Ret(Args...)>;

        Signal() = default;
        ~Signal()
        {
            disconnectAll();
        }

        Signal(const Signal&) = delete;
        Signal& operator=(const Signal&) = delete;

    
        Connection connect(Callback cb)
        {
            size_t id = m_nextId++;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_observers[id] = std::move(cb);
            }

            return Connection(
                [this, id]()
                {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    m_observers.erase(id);
                }
            );
        }

        void disconnectAll() noexcept
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_observers.clear();
        }

        void emit(Args... args)
        {
            //std::map<size_t, Callback> temp_observers;
            //{
            //    std::lock_guard<std::mutex> lock(m_mutex);
            //    temp_observers = m_observers; 
            //                                  
            //}

            if (m_observers.empty())
            {
                return;
            }

            for (auto const& [id, cb] : m_observers)
            {
                if (cb != nullptr)
                {
                    cb(args...);
                }
            }
        }

    private:
        std::map<size_t, Callback> m_observers;
        std::atomic<size_t> m_nextId{0};
        std::mutex m_mutex; 
    };



template <
    typename Publisher,
    typename SignalType,
    typename Subscriber,
    typename Method>
Connection subscribe(
    Publisher& publisher,
    SignalType Publisher::* signal,
    Subscriber& subscriber,
    Method method
)
{
    return (publisher.*signal)
        .connect(
            [&subscriber, method](auto&&... args)
            {
                return (subscriber.*method)(std::forward<decltype(args)>(args)...);
            }
        );
}

template <
    typename Publisher,
    typename SignalType,
    typename Func>
Connection subscribe(
    Publisher& publisher,
    SignalType Publisher::* signal,
    Func&& func
)
{
    return (publisher.*signal).connect(std::forward<Func>(func));
}

template <
    typename Publisher,
    typename Owner,
    typename SignalType,
    typename Func>
Connection subscribe(
    Publisher&& publisher,
    SignalType Owner::* signal,
    Func&& func
)
{
    using PubType = std::remove_pointer_t<std::decay_t<Publisher>>;
    if constexpr (std::is_pointer_v<std::decay_t<Publisher>>)
    {
        return (static_cast<Owner*>(publisher)->*signal).connect(std::forward<Func>(func));
    }
    else
    {
        return (static_cast<Owner&>(publisher).*signal).connect(std::forward<Func>(func));
    }
}

#endif 