#pragma once

#include <string>

namespace u32
{
    int atoi(const char32_t* str);
    long atol(const char32_t* str);
    long long atoll(const char32_t* str);

    int strtoi(const char32_t* str, char32_t **endptr, int base);

    long strtol(const char32_t* str, char32_t** endptr, int base);
    unsigned long strtoul(const char32_t* str, char32_t** endptr, int base);

    long long strtoll(const char32_t* str, char32_t** endptr, int base);
    unsigned long long strtoull(const char32_t* str, char32_t** endptr, int base);

    int stoi(const std::u32string& str, size_t* idx = nullptr, int base = 10);

    long stol(const std::u32string& str, size_t* idx = nullptr, int base = 10);
    unsigned long stoul(const std::u32string& str, size_t* idx = nullptr, int base = 10);

    long long stoll(const std::u32string& str, size_t* idx = nullptr, int base = 10);
    unsigned long long stoull(const std::u32string& str, size_t* idx = nullptr, int base = 10);

    double atof(const char32_t* str);

    float strtof(const char32_t* str, char32_t** endptr);
    double strtod(const char32_t* str, char32_t** endptr);
    long double strtold(const char32_t* str, char32_t** endptr);

    float stof(const std::u32string& str, size_t* idx = nullptr);
    double stod(const std::u32string& str, size_t* idx = nullptr);
    long double stold(const std::u32string& str, size_t* idx = nullptr);
}
