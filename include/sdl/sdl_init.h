//
// Created by stuhlmeier on 11/27/19.
//

#ifndef LIBMUSUBI_SDL_SDL_INIT_H
#define LIBMUSUBI_SDL_SDL_INIT_H

namespace musubi::sdl {
    void init() noexcept;

    void dispose() noexcept;

    struct scoped_init final {
        scoped_init() noexcept;

        ~scoped_init() noexcept;
    };
}

#endif //LIBMUSUBI_SDL_SDL_INIT_H
