/// @file
/// @author stuhlmeier
/// @date 27 November 2019

#ifndef MUSUBI_EXCEPTION_H
#define MUSUBI_EXCEPTION_H

#include <exception>
#include <stdexcept>

namespace musubi {
    /// @brief A framework-internal @ref logic_error.
    /// @details If this exception is thrown, this indicates that some internal invariant
    /// has been violated.
    class assertion_error : public std::logic_error {
    public:
        using std::logic_error::logic_error;

        /// @brief Constructs an @ref assertion_error with no message.
        assertion_error();
    };

    /// @brief A @ref logic_error indicating that some public invariant has been violated.
    /// @details
    /// If this exception is thrown, this indicates that the application
    /// was not in an appropriate state to perform the desired operation.
    class illegal_state_error : public std::logic_error {
    public:
        using std::logic_error::logic_error;
    };

    /// @brief The base exception type for all application @ref runtime_error "runtime_errors".
    class application_error : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    /// @brief An @ref application_error indicating that an attempt to read a resource into memory has failed.
    class resource_read_error : public application_error {
    public:
        using application_error::application_error;
    };

    /// @brief A @ref resource_read_error indicating that an attempt to read from an archive on disk has failed.
    /// @details This may occur when reading an invalid, modified or corrupt @ref asset_registry::mpack "asset pack".
    class archive_read_error : public resource_read_error {
    public:
        using resource_read_error::resource_read_error;
    };

    /// @brief An @ref application_error indicating that an error occurred while loading an asset from a resource pack.
    /// @see asset_registry::mpack
    /// @see asset_loader
    class asset_load_error : public application_error {
    public:
        using application_error::application_error;

        /// @brief Indicates that an asset could not be loaded because a resource had no associated loaded buffer.
        /// @param itemName the name of the resource
        /// @return a new exception
        static asset_load_error no_buffer(std::string_view itemName) noexcept;
    };
}

#endif //MUSUBI_EXCEPTION_H
