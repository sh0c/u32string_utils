#include "u32strlib.h"

#include <cctype>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace u32
{
    namespace
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

        template<typename T>
        T str_to_number(const char32_t* str, char32_t** endptr, int base)
        {
            const char32_t *p = str, *endp {};
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
                *endptr = const_cast<char32_t*>(endp);
            }
            if (overflow)
            {
                return ((is_neg && std::is_signed<T>::value) ? std::numeric_limits<T>::min() : std::numeric_limits<T>::max());
            }

            return ((is_neg) ? -n : n);
        }

        template<typename T>
        typename std::enable_if<std::is_integral<T>::value, T>::type s_to_integral(const std::u32string &str, size_t* idx, int base)
        {
            const auto start = str.c_str();
            char32_t* end {};
            auto result = str_to_number<T>(start, &end, base);
            if (end - start == 0)
            {
                throw std::invalid_argument("no number find in string");
            }

            if (errno == ERANGE)
            {
                throw std::out_of_range("out of range number");
            }

            if (idx)
            {
                *idx = end - start;
            }

            return result;
        }

        template<typename T>
        typename std::enable_if<std::is_floating_point<T>::value, T>::type str_to_floating_point(const char32_t* str, char32_t** endptr)
        {
            const char32_t *p = str;
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

            char32_t* endp {};
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
                *endptr = const_cast<char32_t*>(p);
            }

            return ((is_neg) ? -result : result);
        }
    }

    int atoi(const char32_t* str)
    {
        return str_to_number<int>(str, nullptr, 10);
    }

    long atol(const char32_t* str)
    {
        return str_to_number<long>(str, nullptr, 10);
    }

    long long atoll(const char32_t* str)
    {
        return str_to_number<long long>(str, nullptr, 10);
    }

    int strtoi(const char32_t* str, char32_t **endptr, int base)
    {
        return str_to_number<int>(str, endptr, base);
    }

    long strtol(const char32_t* str, char32_t **endptr, int base)
    {
        return str_to_number<long>(str, endptr, base);
    }

    unsigned long strtoul(const char32_t* str, char32_t** endptr, int base)
    {
        return str_to_number<unsigned long>(str, endptr, base);
    }

    long long strtoll(const char32_t* str, char32_t **endptr, int base)
    {
        return str_to_number<long long>(str, endptr, base);
    }

    unsigned long long strtoull(const char32_t* str, char32_t** endptr, int base)
    {
        return str_to_number<unsigned long long>(str, endptr, base);
    }

    int stoi(const std::u32string& str, size_t* idx, int base)
    {
        return s_to_integral<int>(str, idx, base);
    }

    long stol(const std::u32string& str, size_t* idx, int base)
    {
        return s_to_integral<long>(str, idx, base);
    }

    unsigned long stoul(const std::u32string& str, size_t* idx, int base)
    {
        return s_to_integral<unsigned long>(str, idx, base);
    }

    long long stoll(const std::u32string& str, size_t* idx, int base)
    {
        return s_to_integral<long long>(str, idx, base);
    }

    unsigned long long stoull(const std::u32string& str, size_t* idx, int base)
    {
        return s_to_integral<unsigned long long>(str, idx, base);
    }

    double atof(const char32_t* str)
    {
        return str_to_floating_point<double>(str, nullptr);
    }

    float strtof(const char32_t* str, char32_t** endptr)
    {
        return str_to_floating_point<float>(str, endptr);
    }

    double strtod(const char32_t* str, char32_t** endptr)
    {
        return str_to_floating_point<double>(str, endptr);
    }

    long double strtold(const char32_t* str, char32_t** endptr)
    {
        return str_to_floating_point<long double>(str, endptr);
    }
}
