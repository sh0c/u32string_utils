#pragma once

#include <string>

namespace u32
{
    inline constexpr char32_t get_unicode_invalid_codepoint_char_id()
    {
        return 0xFFFD;
    }

    inline constexpr char32_t get_unicode_max_codepoint_char_id()
    {
        return 0x10FFFF;
    }

    size_t get_utf8_char_len(const char* text);

    size_t utf8_to_u32(char32_t* out_char, const char* in_text, const char* in_text_end);
    size_t utf8_to_u32_reverse(char32_t* out_char, const char* start_str, const char* curr_pos);

    std::string u32_to_utf8(char32_t ch);

    size_t utf16_to_u32(char32_t* out_char, const char16_t* in_text, const char16_t* in_text_end);
    std::u16string u32_to_utf16(char32_t ch);

    std::string convert(const std::u32string& str);
    std::u32string convert(const std::string& str);
}
