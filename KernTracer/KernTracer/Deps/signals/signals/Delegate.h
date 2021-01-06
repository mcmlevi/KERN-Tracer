#pragma once
#ifdef PLATFORM_WIN64
#pragma warning(push,0)
#pragma warning(disable: 4100)
#elif PLATFORM_PS4
#pragma GCC diagnostic  push
#pragma GCC diagnostic  ignored "-Wreorder"
#endif

#include <signals/signals/signals.hpp>
#ifdef PLATFORM_WIN64
#pragma warning(pop)
#elif PLATFORM_PS4
#pragma GCC diagnostic  pop
#endif
namespace SIG
{
    // Delegate that holds function callbacks.
// For simplicity, delegates are limited to void return types.
    template<typename... Args>
    class Delegate
    {
    public:
        Delegate() = default;
        ~Delegate() = default;
        using signal = sig::signal<void(Args...)>;
        using connection = typename signal::connection_type;

        // Adds a function callback to the delegate.
        /**
         * \brief connect a function to the the delegate to call when the delegate is invoked
         * \tparam Func template parameter for the function to bind
         * \param f the function to bind
         * \return the connection type
         */
        template<typename Func>
        connection Connect(Func&& f)
        {
            return m_Callbacks.connect(std::forward<Func>(f));
        }
        /**
         * \brief connect a function to the the delegate to call when the delegate is invoked
         * \tparam Func template parameter for the member function to bind
         * \tparam Instance template parameter for the instance of the class
         * \param f the function to bind
         * \param instance of a class
         * \return the connection type
         */
        template<typename Func, typename Instance>
        connection Connect(Func&& f, Instance&& instance)
        {
            return m_Callbacks.connect(std::forward<Func>(f),instance);
        }
    	
        // Remove a function callback.
        // Returns the number of functions removed.
        /**
         * \brief disconnect function from delegate
         * \tparam Func template parameter for the function to bind
         * \param f function to bind
         * \return 
         */
        template<typename Func>
        std::size_t Disconnect(Func&& f)
        {
            return m_Callbacks.disconnect(std::forward<Func>(f));
        }
        /**
         * \brief disconnect member function from delegate
         * \tparam Func template parameter for the function to bind
         * \tparam Instance template parameter for the class instance
         * \param f the member function to bind
         * \param instance of the class to bind
         * \return 
         */
        template<typename Func, typename Instance>
        std::size_t Disconnect(Func&& f, Instance&& instance)
        {
            return m_Callbacks.disconnect(std::forward<Func>(f), instance);
        }

        /**
         * \brief invoke the delegate
         */
        void operator()(Args&&... args)
        {
            m_Callbacks(std::forward<Args>(args)...);
        }
    private:
        signal m_Callbacks;
    };
}
