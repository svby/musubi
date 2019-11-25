//
// Created by stuhlmeier on 11/23/19.
//

#ifndef LIBMUSUBI_WINDOW_H
#define LIBMUSUBI_WINDOW_H

#include "util/common.h"

namespace musubi {
    class window {
    public:
        using id_type = uint32;

        virtual ~window() = default;

        [[nodiscard]] virtual id_type get_id() const = 0;
    };
}

#endif //LIBMUSUBI_WINDOW_H
