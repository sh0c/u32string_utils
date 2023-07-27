#pragma once

#include <cassert>
#include <cerrno>
#include <stdexcept>
#include <type_traits>
#include <limits>

#include <u32string_utils/u32strlib.h>

template<typename T, typename Func>
void check_stoX(Func f)
{
    {
        std::u32string str(U"  \t\n\r123456 123232");
        size_t end_idx {};
        auto result = f(str.c_str(), &end_idx, 10);
        assert(result == 123456);
        assert(end_idx == 11);
    }

    {
        // dec
        std::u32string str(U"212121A 32323232");
        size_t end_idx {};
        auto result = f(str.c_str(), &end_idx, 10);
        assert(result == 212121);
        assert(end_idx == 6);
    }

    {
        // binary
        std::u32string str(U"10101012 101010111");
        size_t end_idx {};
        auto result = f(str.c_str(), &end_idx, 2);
        assert(result == 0x55);
        assert(end_idx == 7);
    }

    {
        // oct
        std::u32string str(U"01234567888");
        size_t end_idx {};
        auto result = f(str.c_str(), &end_idx, 8);
        assert(result == 01234567);
        assert(end_idx == 8);
    }

    {
        // hex
        std::u32string str(U"0x212121G 32323232");
        size_t end_idx {};
        auto result = f(str.c_str(), &end_idx, 16);
        assert(result == 0x212121);
        assert(end_idx == 8);
    }

    {
        // default oct
        std::u32string str(U"01234567888");
        size_t end_idx {};
        auto result = f(str.c_str(), &end_idx, 0);
        assert(result == 01234567);
        assert(end_idx == 8);
    }

    {
        // default dec
        std::u32string str(U"12348889AA");
        size_t end_idx {};
        auto result = f(str.c_str(), &end_idx, 0);
        assert(result == 12348889);
        assert(end_idx == 8);
    }

    {
        // default hex
        std::u32string str(U"0x212121G 32323232");
        size_t end_idx {};
        auto result = f(str.c_str(), &end_idx, 0);
        assert(result == 0x212121);
        assert(end_idx == 8);
    }

    {
        // max limits parsing
        std::u32string str(U"2000000000000000000000000000000000000 ");
        size_t end_idx {};
        T result {};
        try
        {
            result = f(str.c_str(), &end_idx, 10);
            assert(false);
        }
        catch(const std::out_of_range&)
        {
            assert(true);
        }

        assert(errno == ERANGE);
        assert(result == 0);
        assert(end_idx == 0);
        errno = 0;
    }

    {
        // min limits parsing
        std::u32string str(U"-2000000000000000000000000000000000000 ");
        size_t end_idx {};
        T result {};
        try
        {
            result = f(str.c_str(), &end_idx, 10);
            assert(false);
        }
        catch(const std::out_of_range&)
        {
            assert(true);
        }

        assert(errno == ERANGE);
        assert(result == 0);
        assert(end_idx == 0);
        errno = 0;
    }

    {
        // no parsing
        std::u32string str(U"xxxxxx");
        size_t end_idx {};
        T result {};
        try
        {
            result = f(str.c_str(), &end_idx, 10);
            assert(false);
        }
        catch(const std::invalid_argument&)
        {
            assert(true);
        }

        assert(errno == 0);
        assert(result == 0);
        assert(end_idx == 0);
    }

    {
        // no parsing with whitespaces
        std::u32string str(U"  xxxxxx");
        size_t end_idx {};
        T result {};
        try
        {
            result = f(str.c_str(), &end_idx, 10);
            assert(false);
        }
        catch(const std::invalid_argument&)
        {
            assert(true);
        }

        assert(errno == 0);
        assert(result == 0);
        assert(end_idx == 0);
    }
}
