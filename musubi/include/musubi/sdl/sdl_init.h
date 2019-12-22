//
// Created by stuhlmeier on 11/27/19.
//

#ifndef MUSUBI_SDL_SDL_INIT_H
#define MUSUBI_SDL_SDL_INIT_H

namespace musubi::sdl {
    void init();

    void dispose() noexcept;

    struct scoped_init final {
        scoped_init();

        ~scoped_init() noexcept;
    };
}

#endif //MUSUBI_SDL_SDL_INIT_H
