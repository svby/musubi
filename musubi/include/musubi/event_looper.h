/// @file
/// @author stuhlmeier
/// @date 3 January 2020

#ifndef MUSUBI_EVENT_LOOPER_H
#define MUSUBI_EVENT_LOOPER_H

#include <functional>
#include <vector>

namespace musubi {
    class blocking_looper final {
    public:
        struct stop_token final {
        public:
            friend class blocking_looper;

            void request_stop();

            [[nodiscard]] bool is_stop_requested() const;

        private:
            explicit stop_token(volatile bool &stop);

            volatile bool &stop;
        };

        using action_type = void(stop_token &);

        explicit blocking_looper(std::function<void()> tickFunction);

        blocking_looper &add_action(std::function<action_type> function);

        void request_stop();

        [[nodiscard]] bool is_stop_requested() const;

        void loop();

    private:
        volatile bool stop;

        std::function<void()> updater;
        std::vector<std::function<action_type>> actions;
    };
}

#endif //MUSUBI_EVENT_LOOPER_H
