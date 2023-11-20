#pragma once

namespace u32
{
    char32_t toupper(char32_t ch);

    char32_t tolower(char32_t ch);

    bool toupper_case(char32_t& input);

    bool tolower_case(char32_t& input);

    bool is_upper(char32_t ch);

    bool is_lower(char32_t ch);

    bool is_alpha(char32_t ch);

    bool is_digit(char32_t ch);

    bool is_space(char32_t ch);

    bool is_punctuation(char32_t ch);

    bool is_other_symbol(char32_t ch);

    bool is_currency_symbol(char32_t ch);
}
