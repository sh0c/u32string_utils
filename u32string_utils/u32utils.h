#pragma once

#include <string>

namespace u32
{
    std::string convert(const std::u32string& str);
    std::u32string convert(const std::string& str);

    std::string unicode_to_utf8(const char32_t* in, const char32_t* in_end);
    size_t text_char_from_utf8(char32_t* out_char, const char* in_text, const char* in_text_end);
}
