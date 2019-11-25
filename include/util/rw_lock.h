//
// Created by stuhlmeier on 11/23/19.
//

#ifndef LIBMUSUBI_RW_LOCK_H
#define LIBMUSUBI_RW_LOCK_H

#include <shared_mutex>

namespace musubi {
    class rw_lock final {
    public:
        using write_lock_type = std::unique_lock<std::shared_mutex>;
        using read_lock_type = std::shared_lock<std::shared_mutex>;

        [[nodiscard]] inline auto write_lock() { return write_lock_type{mutex}; }

        [[nodiscard]] inline auto read_lock() { return read_lock_type{mutex}; }

    private:
        std::shared_mutex mutex;
    };
}

#endif //LIBMUSUBI_RW_LOCK_H
