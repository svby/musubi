//
// Created by stuhlmeier on 11/23/19.
//

#ifndef LIBMUSUBI_WINDOW_H
#define LIBMUSUBI_WINDOW_H

#include "musubi/common.h"
#include "musubi/screen.h"

#include <memory>

namespace musubi {
    struct input_state;

    enum class window_mode : uint8 {
        windowed,
        fullscreen,
        minimized,
        maximized
    };

    class window {
    public:
        struct start_info final {
            static constexpr uint32 UNDEFINED_X = std::numeric_limits<uint32>::max();
            static constexpr uint32 UNDEFINED_Y = UNDEFINED_X;

            const std::string &title;

            uint32 width{0u};
            uint32 height{0u};

            uint32 x{UNDEFINED_X};
            uint32 y{UNDEFINED_Y};

            bool hidden{false};
            bool undecorated{false};
            bool resizable{false};
            window_mode mode{window_mode::windowed};
        };

        using id_type = uint32;

        virtual ~window() = default;

        virtual void set_screen(std::shared_ptr<screen> newScreen) = 0;

        virtual void update(const input_state &inputState) = 0;

        [[nodiscard]] virtual id_type get_id() const = 0;
    };
}

#endif //LIBMUSUBI_WINDOW_H
