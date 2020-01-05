/// @file
/// @author stuhlmeier
/// @date 19 December 2019

#ifndef MUSUBI_SCREEN_H
#define MUSUBI_SCREEN_H

namespace musubi {
    class window;

    /// @brief A common interface for _screens_.
    /// @details
    /// Screens are essentially tick callbacks with a lifecycle, allowing for easier memory management.
    /// They are owned by @ref window "windows" and are updated on every tick.
    /// @see on_attached(), on_update(), on_detached()
    class screen {
    public:
        /// @details Destroys this screen and releases its resources.
        virtual ~screen();

        /// @brief Called when this screen has been attached to a @ref window.
        /// @param window the window this screen was attached to
        virtual void on_attached(window *window) = 0;

        /// @brief Called when the @ref application performs a tick.
        /// @param dt the delta time since the last tick
        virtual void on_update(float dt) = 0;

        /// @brief Called when this screen has been removed from a @ref window.
        /// @param window the window this screen was detached from
        virtual void on_detached(window *window) = 0;
    };

    /// @brief An extensible @ref screen implementation with a no-op update callback.
    /// @details The various screen callbacks can be overridden to provide functionality.
    /// Overrides of @ref on_attached and @ref on_detached should begin with a call to this class's implementation,
    /// as they set the necessary internal state for @ref get_current_window to work properly.
    /// @see screen
    class basic_screen : public screen {
    public:
        /// @copydoc ~screen();
        ~basic_screen() override;

        void on_attached(window *window) override;

        void on_update(float dt) override;

        void on_detached(window *window) override;

    protected:
        /// @details Retrieves a pointer to the window this screen is attached to.
        /// @return a pointer to the current window, or null
        [[nodiscard]] window *get_current_window() const;

    private:
        window *currentWindow;
    };
}

#endif //MUSUBI_SCREEN_H
