#pragma once

#include <cctype>
#include <cerrno>
#include <limits>
#include <string>
#include <type_traits>

namespace details
{
    template<typename T>
    typename std::enable_if<std::is_floating_point<T>::value, T>::type get_cutoff(bool, int base)
    {
        return std::numeric_limits<T>::max() / base;
    }

    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, T>::type get_cutoff(bool is_negative, int base)
    {
        if (is_negative && std::is_signed<T>::value)
        {
            return -(std::numeric_limits<T>::min() / base);
        }
        return std::numeric_limits<T>::max() / base;
    }

    template<typename T>
    typename std::enable_if<std::is_floating_point<T>::value, T>::type get_cutlimit(bool, int base)
    {
        return std::numeric_limits<T>::max() - std::numeric_limits<T>::max() / base;
    }

    template<typename T>
    typename std::enable_if<std::is_integral<T>::value, T>::type get_cutlimit(bool is_negative, int base)
    {
        if (is_negative && std::is_signed<T>::value)
        {
            return -(std::numeric_limits<T>::min() % base);
        }
        return std::numeric_limits<T>::max() % base;
    }

    template<typename T, typename charT>
    T str_to_number(const charT* str, charT** endptr, int base)
    {
        const charT *p = str, *endp {};
        bool is_neg {}, overflow {};
        T n {};

        if (base < 0 || base == 1 || base > 36)
        {
            return {};
        }

        endp = str;

        while (isspace(int(*p)))
        {
            p++;
        }

        if (*p == '+')
        {
            p++;
        }
        else if (*p == '-')
        {
            is_neg = true, p++;
        }

        if (*p == '0')
        {
            p++;
            /* For (" 0xZ", &endptr, 16), endptr should point to 'x';
                     * pointing to ' ' or '0' is non-compliant.
                     * (Many implementations do this wrong.) */
            endp = p;
            if (base == 16 && (*p == 'X' || *p == 'x'))
            {
                p++;
            }
            else if (base == 0)
            {
                if (*p == 'X' || *p == 'x')
                {
                    base = 16, p++;
                }
                else
                {
                    base = 8;
                }
            }
        }
        else if (base == 0)
        {
            base = 10;
        }

        auto cutoff = get_cutoff<T>(is_neg, base);
        auto cutlim = get_cutlimit<T>(is_neg, base);

        while (true)
        {
            int c {};
            if (*p >= 'A')
            {
                c = ((*p - 'A') & (~('a' ^ 'A'))) + 10;
            }
            else if (*p <= '9')
            {
                c = *p - '0';
            }
            else
            {
                break;
            }

            if (c < 0 || c >= base)
            {
                break;
            }

            endp = ++p;

            if (overflow)
            {
                /* set overflow errno */
                errno = ERANGE;
                /* endptr should go forward and point to the non-digit character
                         * (of the given base); required by ANSI standard. */
                if (endptr)
                {
                    continue;
                }
                break;
            }

            if (n > cutoff || (n == cutoff && T(c) > cutlim))
            {
                overflow = true;
                continue;
            }

            n = n * base + c;
        }
        if (endptr)
        {
            *endptr = const_cast<charT*>(endp);
        }
        if (overflow)
        {
            return ((is_neg && std::is_signed<T>::value) ? std::numeric_limits<T>::min() : std::numeric_limits<T>::max());
        }

        return ((is_neg) ? -n : n);
    }



}
