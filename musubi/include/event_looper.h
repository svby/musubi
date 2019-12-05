//
// Created by stuhlmeier on 11/24/19.
//

#ifndef LIBMUSUBI_EVENT_LOOPER_H
#define LIBMUSUBI_EVENT_LOOPER_H

#include <functional>
#include <vector>

namespace musubi {
    class event_looper {
    public:
        struct stop_token final {
            virtual ~stop_token() = default;

            void request_stop() volatile;

            [[nodiscard]] bool is_stop_requested() volatile const;

        private:
            bool stop{false};
        };

        void request_stop();

        [[nodiscard]] bool is_stop_requested() const;

        [[nodiscard]] volatile stop_token &get_stop_token();

        [[nodiscard]] volatile const stop_token &get_stop_token() const;

    protected:
        volatile stop_token token;
    };

    class blocking_looper final : public virtual event_looper {
    public:
        explicit blocking_looper(std::function<void()> updater);

        blocking_looper &add_action(std::function<void(volatile stop_token &)> functor);

        ~blocking_looper();

        void loop();

    private:
        std::function<void()> updater;
        std::vector<std::function<void(volatile stop_token &)>> functors;
    };
}

#endif //LIBMUSUBI_EVENT_LOOPER_H
