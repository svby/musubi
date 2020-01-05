/// @file
/// @author stuhlmeier
/// @date 3 January 2020

#ifndef MUSUBI_EVENT_LOOPER_H
#define MUSUBI_EVENT_LOOPER_H

#include <functional>
#include <vector>

namespace musubi {
    /// @brief An event looper that loops infinitely on the calling thread.
    /// @details This is best suited for applications handling input on the main thread (i.e. most applications.)
    ///
    /// Calling @ref loop() will block forever, so this is typically called after initial setup.
    /// To stop the looper and the application, @ref request_stop can be invoked on an event looper,
    /// or on one of its @ref stop_token "stop_tokens".
    ///
    /// Loopers are constructed with a specified tick function, which is executed on every iteration;
    /// @ref action_type "actions" can be added to be executed after the tick function.
    class blocking_looper final {
    public:
        /// @brief A stop token for a @ref blocking_looper.
        /// @details This class provides a view of the looper's stop flag,
        /// and can be used to stop the associated looper.
        struct stop_token final {
        public:
            friend class blocking_looper;

            /// @brief Sets the associated @ref blocking_looper's stop flag.
            /// @details The effect of this function is equivalent to invoking
            /// @ref blocking_looper::request_stop() "request_stop()" on the associated @ref blocking_looper.
            void request_stop();

            /// @brief Retrieves the associated @ref blocking_looper's stop flag.
            /// @return the associated looper's stop flag
            [[nodiscard]] bool is_stop_requested() const;

        private:
            explicit stop_token(volatile bool &stop);

            volatile bool &stop;
        };

        /// @brief The function type for actions that can be added to this looper for execution.
        /// @see add_action()
        /// @see blocking_looper
        using action_type = void(stop_token &);

        /// @brief Constructs a looper with the specified tick function.
        /// @param tickFunction the tick function
        /// @see blocking_looper
        explicit blocking_looper(std::function<void()> tickFunction);

        /// @brief Adds the specified action to this looper for execution.
        /// @param function the function to be executed on every tick
        /// @return this, for chaining
        /// @see blocking_looper
        blocking_looper &add_action(std::function<action_type> function);

        /// @brief Sets this looper's stop flag.
        /// @details After the current action completes, the looper will terminate.
        void request_stop();

        /// @brief Retrieves this looper's stop flag.
        /// @return this looper's stop flag
        [[nodiscard]] bool is_stop_requested() const;

        /// @brief Starts the event loop in the current thread, blocking until completion.
        void loop();

    private:
        volatile bool stop;

        std::function<void()> updater;
        std::vector<std::function<action_type>> actions;
    };
}

#endif //MUSUBI_EVENT_LOOPER_H
