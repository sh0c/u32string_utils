#pragma once

#include <cassert>
#include <cerrno>
#include <type_traits>
#include <limits>

#include <u32string_utils/u32strlib.h>

template<typename T, typename Func>
void check_strtoX(Func f)
{
    {
        std::u32string str(U"  \t\n\r123456 123232");
        char32_t* endptr {};
        auto result = f(str.c_str(), &endptr, 10);
        assert(result == 123456);
        assert(endptr == str.c_str() + 11);
    }

    {
        // dec
        std::u32string str(U"212121A 32323232");
        char32_t* endptr {};
        auto result = f(str.c_str(), &endptr, 10);
        assert(result == 212121);
        assert(endptr == str.c_str() + 6);
    }

    {
        // binary
        std::u32string str(U"10101012 101010111");
        char32_t* endptr {};
        auto result = f(str.c_str(), &endptr, 2);
        assert(result == 0x55);
        assert(endptr == str.c_str() + 7);
    }

    {
        // oct
        std::u32string str(U"01234567888");
        char32_t* endptr {};
        auto result = f(str.c_str(), &endptr, 8);
        assert(result == 01234567);
        assert(endptr == str.c_str() + 8);
    }

    {
        // hex
        std::u32string str(U"0x212121G 32323232");
        char32_t* endptr {};
        auto result = f(str.c_str(), &endptr, 16);
        assert(result == 0x212121);
        assert(endptr == str.c_str() + 8);
    }

    {
        // default oct
        std::u32string str(U"01234567888");
        char32_t* endptr {};
        auto result = f(str.c_str(), &endptr, 0);
        assert(result == 01234567);
        assert(endptr == str.c_str() + 8);
    }

    {
        // default dec
        std::u32string str(U"12348889AA");
        char32_t* endptr {};
        auto result = f(str.c_str(), &endptr, 0);
        assert(result == 12348889);
        assert(endptr == str.c_str() + 8);
    }

    {
        // default hex
        std::u32string str(U"0x212121G 32323232");
        char32_t* endptr {};
        auto result = f(str.c_str(), &endptr, 0);
        assert(result == 0x212121);
        assert(endptr == str.c_str() + 8);
    }

    {
        // max limits parsing
        std::u32string str(U"2000000000000000000000000000000000000 ");
        char32_t* endptr {};
        auto result = f(str.c_str(), &endptr, 10);
        assert(result == std::numeric_limits<T>::max());
        assert(errno == ERANGE);
        assert(endptr == str.c_str() + 37);
        errno = 0;
    }

    if (std::is_unsigned<T>::value)
    {
        // min limits parsing
        std::u32string str(U"-2000000000000000000000000000000000000 ");
        char32_t* endptr {};
        auto result = f(str.c_str(), &endptr, 10);
        assert(result == std::numeric_limits<T>::max());
        assert(errno == ERANGE);
        assert(endptr == str.c_str() + 38);
        errno = 0;
    }
    else
    {
        // min limits parsing
        std::u32string str(U"-2000000000000000000000000000000000000 ");
        char32_t* endptr {};
        auto result = f(str.c_str(), &endptr, 10);
        assert(result == std::numeric_limits<T>::min());
        assert(errno == ERANGE);
        assert(endptr == str.c_str() + 38);
        errno = 0;
    }

    {
        // no parsing
        std::u32string str(U"xxxxxx");
        char32_t* endptr {};
        auto result = f(str.c_str(), &endptr, 10);
        assert(result == 0);
        assert(str.c_str() == endptr);
        assert(errno == 0);
    }

    {
        // no parsing with whitespaces
        std::u32string str(U"  xxxxxx");
        char32_t* endptr {};
        auto result = f(str.c_str(), &endptr, 10);
        assert(result == 0);
        assert(str.c_str() == endptr);
        assert(errno == 0);
    }
}
