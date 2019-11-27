//
// Created by stuhlmeier on 11/27/19.
//

#ifndef LIBMUSUBI_EXCEPTION_H
#define LIBMUSUBI_EXCEPTION_H

#include <exception>
#include <stdexcept>

namespace musubi {
    class assertion_error : public std::logic_error {
    public:
        using std::logic_error::logic_error;

        assertion_error();
    };

    class application_error : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };
}

#endif //LIBMUSUBI_EXCEPTION_H