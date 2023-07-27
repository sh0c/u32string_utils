#pragma once

#include <cassert>
#include <cerrno>
#include <type_traits>
#include <limits>

#include "real_equal.h"

template<typename T, typename Func>
void check_atoX_real(Func f)
{
    {
        // default parsing
        auto result = f(U"12.3456789");
        assert(equal(result, 12.3456789));
    }

    {
        // parsing with symbol +
        auto result = f(U"+12.345678");
        assert(equal(result, 12.345678));
    }

    {
        // default with symbol -
        auto result = f(U"-12.345678");
        assert(equal(result, -12.345678));
    }

    {
        // max limits parsing
        auto result = f(U"1.9e+320");
        assert(equal(result, std::numeric_limits<T>::max()));
        assert(errno == ERANGE);
        errno = 0;
    }

    {
        // min limits parsing
        auto result = f(U"-1.9e+320");
        assert(equal(result, -std::numeric_limits<T>::max()));
        assert(errno == ERANGE);
        errno = 0;
    }

    {
        // with whitespaces
        auto result = f(U"  \t\n\r12.3456");
        assert(equal(result, 12.3456));
    }

    {
        // no parsing
        auto result = f(U"xxxxxxx");
        assert(equal(result, 0.0));
        assert(errno == 0);
    }
}
