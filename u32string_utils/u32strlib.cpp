#include "u32strlib.h"

#include "details/str_to_floating_point.h"

#include <stdexcept>

namespace u32
{
    namespace
    {
        template<typename T, typename charT>
        typename std::enable_if<std::is_integral<T>::value, T>::type s_to_integral(const std::basic_string<charT> &str, size_t* idx, int base)
        {
            const auto start = str.c_str();
            charT* end {};
            auto result = details::str_to_number<T>(start, &end, base);
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

        template<typename T, typename charT>
        typename std::enable_if<std::is_floating_point<T>::value, T>::type s_to_floating_point(const std::basic_string<charT> &str, size_t* idx)
        {
            const auto start = str.c_str();
            charT* end {};
            auto result = details::str_to_floating_point<T>(start, &end);
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
    }

    int atoi(const char32_t* str)
    {
        return details::str_to_number<int, char32_t>(str, nullptr, 10);
    }

    long atol(const char32_t* str)
    {
        return details::str_to_number<long, char32_t>(str, nullptr, 10);
    }

    long long atoll(const char32_t* str)
    {
        return details::str_to_number<long long, char32_t>(str, nullptr, 10);
    }

    int strtoi(const char32_t* str, char32_t **endptr, int base)
    {
        return details::str_to_number<int>(str, endptr, base);
    }

    long strtol(const char32_t* str, char32_t **endptr, int base)
    {
        return details::str_to_number<long>(str, endptr, base);
    }

    unsigned long strtoul(const char32_t* str, char32_t** endptr, int base)
    {
        return details::str_to_number<unsigned long>(str, endptr, base);
    }

    long long strtoll(const char32_t* str, char32_t **endptr, int base)
    {
        return details::str_to_number<long long>(str, endptr, base);
    }

    unsigned long long strtoull(const char32_t* str, char32_t** endptr, int base)
    {
        return details::str_to_number<unsigned long long>(str, endptr, base);
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
        return details::str_to_floating_point<double, char32_t>(str, nullptr);
    }

    float strtof(const char32_t* str, char32_t** endptr)
    {
        return details::str_to_floating_point<float>(str, endptr);
    }

    double strtod(const char32_t* str, char32_t** endptr)
    {
        return details::str_to_floating_point<double>(str, endptr);
    }

    long double strtold(const char32_t* str, char32_t** endptr)
    {
        return details::str_to_floating_point<long double>(str, endptr);
    }

    float stof(const std::u32string& str, size_t* idx)
    {
        return s_to_floating_point<float>(str, idx);
    }

    double stod(const std::u32string& str, size_t* idx)
    {
        return s_to_floating_point<double>(str, idx);
    }

    long double stold(const std::u32string& str, size_t* idx)
    {
        return s_to_floating_point<long double>(str, idx);
    }
}
