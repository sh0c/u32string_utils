#pragma once

#include <cstdint>
#include <type_traits>

namespace details
{
    template<typename T>
    inline std::size_t chars_len(T val, int base) noexcept
    {
        static_assert(std::is_integral<T>::value, "type is not integral");

        std::size_t n = 1;
        const std::size_t b2 = base  * base;
        const std::size_t b3 = b2 * base;
        const std::size_t b4 = b3 * base;

        for (;;)
        {
            if (val < base) return n;
            if (val < b2) return n + 1;
            if (val < b3) return n + 2;
            if (val < b4) return n + 3;
            val /= b4;
            n += 4;
        }
    }

    template<typename T, typename charT>
    inline std::size_t to_chars(charT* start, charT* end, T val, int base = 10) noexcept
    {
        static_assert(std::is_integral<T>::value, "type is not integral");

        const auto len = chars_len(val, base);
        if (end - start < len)
        {
            return {};
        }

        constexpr charT digits[201] {'0','0','0','1','0','2','0','3','0','4','0','5','0','6','0','7','0','8','0','9',
                                     '1','0','1','1','1','2','1','3','1','4','1','5','1','6','1','7','1','8','1','9',
                                     '2','0','2','1','2','2','2','3','2','4','2','5','2','6','2','7','2','8','2','9',
                                     '3','0','3','1','3','2','3','3','3','4','3','5','3','6','3','7','3','8','3','9',
                                     '4','0','4','1','4','2','4','3','4','4','4','5','4','6','4','7','4','8','4','9',
                                     '5','0','5','1','5','2','5','3','5','4','5','5','5','6','5','7','5','8','5','9',
                                     '6','0','6','1','6','2','6','3','6','4','6','5','6','6','6','7','6','8','6','9',
                                     '7','0','7','1','7','2','7','3','7','4','7','5','7','6','7','7','7','8','7','9',
                                     '8','0','8','1','8','2','8','3','8','4','8','5','8','6','8','7','8','8','8','9',
                                     '9','0','9','1','9','2','9','3','9','4','9','5','9','6','9','7','9','8','9','9',};

        std::size_t pos = len - 1;
        while (val >= 100)
        {
            auto const num = (val % 100) * 2;
            val /= 100;
            start[pos] = digits[num + 1];
            start[pos - 1] = digits[num];
            pos -= 2;
        }

        if (val >= 10)
        {
            auto const num = val * 2;
            start[1] = digits[num + 1];
            start[0] = digits[num];
        }
        else
        {
            start[0] = '0' + val;
        }

        return len;
    }
}
