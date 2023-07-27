#pragma once

#include <cassert>
#include <type_traits>
#include <limits>

#include <cerrno>


template<typename T, typename Func>
void check_atoX_integral(Func f)
{
    {
        // default parsing
        auto result = f(U"12345678");
        assert(result == 12345678);
    }

    {
        // parsing with symbol +
        auto result = f(U"+12345678");
        assert(result == 12345678);
    }

    {
        // default with symbol -
        auto result = f(U"-12345678");
        assert(result == -12345678);
    }

    {
        // max limits parsing
        auto result = f(U"2000000000000000000000000000000000000");
        assert(result == std::numeric_limits<T>::max());
        assert(errno == ERANGE);
        errno = 0;
    }

    {
        // min limits parsing
        auto result = f(U"-2000000000000000000000000000000000000");
        assert(result == std::numeric_limits<T>::min());
        assert(errno == ERANGE);
        errno = 0;
    }

    {
        // parsing hex
        auto result = f(U"0x2299AAG");
        assert(result == 0);
    }

    {
        // parsing oct is actualy skip 0 in begin
        auto result = f(U"0332378");
        assert(result == 332378);
    }

    {
        // with whitespaces
        auto result = f(U"  \t\n\r123456");
        assert(result == 123456);
    }

    {
        // no parsing
        auto result = f(U"xxxxxxx");
        assert(result == 0);
        assert(errno == 0);
    }
}
