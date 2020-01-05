/// @file
/// @author stuhlmeier
/// @date 23 November 2019

#ifndef MUSUBI_WINDOW_H
#define MUSUBI_WINDOW_H

#include "musubi/common.h"
#include "musubi/screen.h"

#include <memory>

namespace musubi {
    struct input_state;

    /// @brief The display mode of a window.
    enum class window_mode : uint8 {
        windowed, ///< Windowed display mode
        fullscreen, ///< Fullscreen display mode
        borderless, ///< Borderless fullscreen (fullscreen-desktop) display mode
    };

    /// @brief An application window.
    /// @details
    /// Windows should handle their own lifecycle; they are destroyed when the associated object is deleted.
    ///
    /// A @ref screen can be attached to handle updates and render content.
    ///
    /// Windows created through an @ref application instance will be automatically registered,
    /// as will their associated @ref input_poller "input_pollers".
    /// Once registered, input is polled once per tick and passed to all windows via @ref update().
    ///
    /// For a window implementation to be registrable by an application, it may define a member typedef `poller_type`.
    /// @see application
    class window {
    public:
        /// @brief A window's initialization parameters.
        struct start_info final {
            /// @brief A constant integral value representing an unknown initial X position.
            static constexpr uint32 UNDEFINED_X = std::numeric_limits<uint32>::max();
            /// @brief A constant integral value representing an unknown initial Y position.
            static constexpr uint32 UNDEFINED_Y = UNDEFINED_X;

            std::string title; ///< @brief The window's initial title.

            uint32 width{0u}; ///< @brief The window's initial width.
            uint32 height{0u}; ///< @brief The window's initial height.

            uint32 x{UNDEFINED_X}; ///< @brief The initial window X position, or @ref UNDEFINED_X.
            uint32 y{UNDEFINED_Y}; ///< @brief The initial window Y position, or @ref UNDEFINED_Y.

            bool hidden{false}; ///< @brief Whether the window is initially hidden.
            bool undecorated{false}; ///< @brief Whether the window is initially undecorated.
            bool resizable{false}; ///< @brief Whether the window is initially resizable.
            window_mode mode{window_mode::windowed}; ///< @brief The window's initial @ref window_mode "window mode".
        };

        /// @brief The unique ID type used internally to identify windows.
        /// @details
        /// Window identifiers must be globally unique, regardless of what framework managed or created them.
        /// Since many libraries return an internal ID that
        /// does not necessarily respond to the actual system window handle,
        /// simply using the respective framework window ID may result in identifier collisions.
        // TODO avoid this by additionally storing the window type_index along with the id_type
        using id_type = uint32;

        /// @brief Closes and destroys this window.
        virtual ~window() = default;

        /// @brief Sets this window's current @ref screen.
        /// @details The old screen instance is destroyed.
        ///
        /// Window implementations should extend the lifetime of the current screen
        /// to after the current tick has finished, so that setting a new screen
        /// from the current screen's update callback will not destroy the screen before it is finished.
        /// @param newScreen the new screen
        virtual void set_screen(std::unique_ptr<screen> newScreen) = 0;

        /// @brief Performs a tick.
        /// @details This function is typically called from the application's event loop.
        /// @param inputState the current window input state
        virtual void update(const input_state &inputState) = 0;

        /// @details Retrieves this window's internal identifier.
        /// @return this window's ID
        [[nodiscard]] virtual id_type get_id() const = 0;
    };
}

#endif //MUSUBI_WINDOW_H
