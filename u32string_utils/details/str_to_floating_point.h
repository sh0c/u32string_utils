#pragma once

#include "str_to_integral.h"

namespace details
{
    template<typename T, typename charT>
    typename std::enable_if<std::is_floating_point<T>::value, T>::type str_to_floating_point(const charT* str, charT** endptr)
    {
        const charT *p = str;
        bool is_neg {};
        int base {10};

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
            is_neg = true;
            p++;
        }

        if (*p == '0')
        {
            p++;
            if (p[1] == 'X' || p[1] == 'x')
            {
                base = 16;
            }
        }

        charT* endp {};
        auto integral = str_to_number<T>(p, &endp, base);
        p = endp;

        if (errno == ERANGE)
        {
            /* errno is set ot overflow */
            return integral;
        }

        T result {integral};
        T fraction {};
        if (*p == '.')
        {
            p++;
            fraction = str_to_number<T>(p, &endp, base);
            while (fraction > T(1))
            {
                fraction /= T(10);
            }
            p = endp;
            result = integral + fraction;
        }
        else if ((*p == 'I' || *p == 'i') &&
                 (p[1] == 'N' || p[1] == 'n') &&
                 (p[2] == 'F' || p[2] == 'f'))
        {
            p += 3;
            if ((*p == 'I' || *p == 'i') &&
                (p[1] == 'N' || p[1] == 'n') &&
                (p[2] == 'I' || p[2] == 'i') &&
                (p[1] == 'T' || p[1] == 't') &&
                (p[2] == 'Y' || p[2] == 'y'))
            {
                p += 5;
            }
            result = std::numeric_limits<T>::max();
        }
        else if ((*p == 'N' || *p == 'n') &&
                 (p[1] == 'A' || p[1] == 'a') &&
                 (p[2] == 'N' || p[2] == 'n'))
        {
            p += 3;
            result = std::numeric_limits<T>::quiet_NaN();
        }


        if (base == 10 && (*p == 'E' || *p == 'e'))
        {
            p++;
            auto exp = str_to_number<int>(p, &endp, base);
            auto abs_exp = abs(exp);
            for (int i = 0; i < abs_exp; i++)
            {
                if (exp < 0)
                {
                    result /= T(10);
                }
                else
                {
                    result *= T(10);
                }
            }

            if (exp < std::numeric_limits<T>::min_exponent10)
            {
                errno = ERANGE;
            }
            else if (exp > std::numeric_limits<T>::max_exponent10)
            {
                errno = ERANGE;
                result = std::numeric_limits<T>::max();
            }

            p = endp;
        }
        else if (base == 16 && *p == 'p')
        {
            p++;
            auto radix = str_to_number<int>(p, &endp, base);
            auto abs_radix = abs(radix);
            for (int i = 0; i < abs_radix; i++)
            {
                if (radix < 0)
                {
                    result /= T(2);
                }
                else
                {
                    result *= T(2);
                }
            }

            if (radix < std::numeric_limits<T>::min_exponent)
            {
                errno = ERANGE;
            }
            else if (radix > std::numeric_limits<T>::max_exponent)
            {
                errno = ERANGE;
                result = std::numeric_limits<T>::max();
            }

            p = endp;
        }

        /* skip end symbols */
        if (*p == 'f' || *p == 'F' || *p == 'l' || *p == 'L')
        {
            p++;
        }

        if (endptr)
        {
            *endptr = const_cast<charT*>(p);
        }

        return ((is_neg) ? -result : result);
    }
}
