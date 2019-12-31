/// @file
/// @author stuhlmeier
/// @date 23 November 2019

#ifndef MUSUBI_UTIL_RW_LOCK_H
#define MUSUBI_UTIL_RW_LOCK_H

#include <shared_mutex>

namespace musubi {
    /// @brief A readers/writer lock backed by a @ref shared_mutex.
    class rw_lock final {
    public:
        /// @brief The lock type returned by @ref write_lock.
        using write_lock_type = std::unique_lock<std::shared_mutex>;
        /// @brief The lock type returned by @ref read_lock.
        using read_lock_type = std::shared_lock<std::shared_mutex>;

        /// @brief Returns a shared read lock.
        /// @return a shared read lock
        [[nodiscard]] inline auto read_lock() { return read_lock_type{mutex}; }

        /// @brief Returns a unique write lock.
        /// @return a unique write lock
        [[nodiscard]] inline auto write_lock() { return write_lock_type{mutex}; }

    private:
        std::shared_mutex mutex;
    };
}

#endif //MUSUBI_UTIL_RW_LOCK_H
