#include "u32utils.h"

namespace u32
{
    namespace
    {
        auto unicode_to_utf8(const char32_t* in, const char32_t* in_end) -> std::string
        {
            std::string out;
            char32_t codepoint = 0;
            for (; in != in_end;  ++in)
            {
                if (*in >= 0xd800 && *in <= 0xdbff)
                {
                    codepoint = ((*in - 0xd800) << 10) + 0x10000;
                }
                else
                {
                    if (*in >= 0xdc00 && *in <= 0xdfff)
                    {
                        codepoint |= *in - 0xdc00;
                    }
                    else
                    {
                        codepoint = *in;
                    }

                    if (codepoint <= 0x7f)
                    {
                        out.append(1, static_cast<char>(codepoint));
                    }
                    else if (codepoint <= 0x7ff)
                    {
                        out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
                        out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
                    }
                    else if (codepoint <= 0xffff)
                    {
                        out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
                        out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
                        out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
                    }
                    else
                    {
                        out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
                        out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
                        out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
                        out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
                    }
                    codepoint = 0;
                }
            }
            return out;
        }

        auto is_surrogate(char16_t ch) -> int
        {
            return (ch - 0xd800u) < 2048u;
        }

        auto is_high_surrogate(char16_t ch) -> int
        {
            return (ch & 0xfffffc00) == 0xd800;
        }

        auto is_low_surrogate(char16_t ch) -> int
        {
            return (ch & 0xfffffc00) == 0xdc00;
        }

        auto surrogate_to_utf32(char16_t high, char16_t low) -> char32_t
        {
            return (high << 10) + low - 0x35fdc00;
        }
    }

    auto utf8_to_u32(char32_t* out_char, const char* in_text, const char* in_text_end) -> size_t
    {
        auto c = static_cast<char32_t>(-1);
        auto str = reinterpret_cast<const uint8_t*>(in_text);
        if(!(*str & 0x80))
        {
            c = static_cast<char32_t>(*str++);
            *out_char = c;
            return 1;
        }
        if((*str & 0xe0) == 0xc0)
        {
            *out_char = get_unicode_invalid_codepoint_char_id(); // will be invalid but not end of string
            if(in_text_end && in_text_end - reinterpret_cast<const char*>(str) < 2)
            {
                return 1;
            }
            if(*str < 0xc2)
            {
                return 2;
            }
            c = static_cast<char32_t>((*str++ & 0x1f) << 6);
            if((*str & 0xc0) != 0x80)
            {
                return 2;
            }
            c += (*str++ & 0x3f);
            *out_char = c;
            return 2;
        }
        if((*str & 0xf0) == 0xe0)
        {
            *out_char = get_unicode_invalid_codepoint_char_id(); // will be invalid but not end of string
            if(in_text_end && in_text_end - reinterpret_cast<const char*>(str) < 3)
            {
                return 1;
            }
            if(*str == 0xe0 && (str[1] < 0xa0 || str[1] > 0xbf))
            {
                return 3;
            }
            if(*str == 0xed && str[1] > 0x9f)
            {
                return 3; // str[1] < 0x80 is checked below
            }
            c = static_cast<char32_t>((*str++ & 0x0f) << 12);
            if((*str & 0xc0) != 0x80)
            {
                return 3;
            }
            c += static_cast<char32_t>((*str++ & 0x3f) << 6);
            if((*str & 0xc0) != 0x80)
            {
                return 3;
            }
            c += (*str++ & 0x3f);
            *out_char = c;
            return 3;
        }
        if((*str & 0xf8) == 0xf0)
        {
            *out_char = get_unicode_invalid_codepoint_char_id(); // will be invalid but not end of string
            if(in_text_end && in_text_end - reinterpret_cast<const char*>(str) < 4)
            {
                return 1;
            }
            if(*str > 0xf4)
            {
                return 4;
            }
            if(*str == 0xf0 && (str[1] < 0x90 || str[1] > 0xbf))
            {
                return 4;
            }
            if(*str == 0xf4 && str[1] > 0x8f)
            {
                return 4; // str[1] < 0x80 is checked below
            }
            c = static_cast<char32_t>((*str++ & 0x07) << 18);
            if((*str & 0xc0) != 0x80)
            {
                return 4;
            }
            c += static_cast<char32_t>((*str++ & 0x3f) << 12);
            if((*str & 0xc0) != 0x80)
            {
                return 4;
            }
            c += static_cast<char32_t>((*str++ & 0x3f) << 6);
            if((*str & 0xc0) != 0x80)
            {
                return 4;
            }
            c += (*str++ & 0x3f);
            // utf-8 encodings of values used in surrogate pairs are invalid
            if((c & 0xFFFFF800) == 0xD800)
            {
                return 4;
            }
            // If codepoint does not fit in ImWchar, use replacement character U+FFFD instead
            if (c > get_unicode_max_codepoint_char_id()) c = get_unicode_invalid_codepoint_char_id();
            *out_char = c;
            return 4;
        }
        *out_char = 0;
        return 0;
    }

    auto utf8_to_u32_reverse(char32_t* out_char, const char* start_str, const char* curr_pos) -> size_t
    {
        auto c = static_cast<char32_t>(-1);

        if (curr_pos - 1 < start_str)
        {
            *out_char = c;
            return 0;
        }

        auto end_pos = curr_pos;

        do
        {
            --curr_pos;
            if(!(*curr_pos & 0x80) || (*curr_pos & 0xe0) == 0xc0 || (*curr_pos & 0xf0) == 0xe0 || (*curr_pos & 0xf8) == 0xf0)
            {
                auto ptr = curr_pos;
                return utf8_to_u32(out_char, ptr, end_pos);
            }

            if(start_str == curr_pos)
            {
                break;
            }
        }
        while(true);

        return 0;
    }

    auto u32_to_utf8(char32_t ch) -> std::string
    {
        return unicode_to_utf8(&ch, &ch + 1);
    }

    auto utf16_to_u32(char32_t *out_char, const char16_t *in_text, const char16_t *in_text_end) -> size_t
    {
        auto uc = *in_text;
        if (!is_surrogate(uc))
        {
            *out_char = uc;
            return 1;
        }
        else
        {
            in_text++;
            if (is_high_surrogate(uc) && in_text < in_text_end && is_low_surrogate(*in_text))
            {
                *out_char = surrogate_to_utf32(uc, *in_text);
                return 2;
            }
            else
            {
                return 0;
            }
        }
    }

    auto u32_to_utf16(char32_t ch) -> std::u16string
    {
        if (ch < 0x10000)
        {
            return std::u16string{char16_t(ch)};
        }

        ch -= 0x10000;
        char16_t arr[2] {};
        arr[0] = char16_t(0xD800 | (ch >> 10));
        arr[1] = char16_t(0xDC00 | (ch & 0x3FF));

        return std::u16string{std::begin(arr), std::end(arr)};
    }

    auto iso_8859_1_to_utf8(std::string &str) -> std::string
    {
        std::string out;
        for (const uint8_t ch : str)
        {
            if (ch < 0x80) {
                out.push_back(ch);
            }
            else {
                out.push_back(0xc0 | ch >> 6);
                out.push_back(0x80 | (ch & 0x3f));
            }
        }
        return out;
    }

    auto convert(const std::u32string &str) -> std::string
    {
        return unicode_to_utf8(str.data(), str.data() + str.size());
    }

    auto convert(const std::string &str) -> std::u32string
    {
        std::u32string result {};
        result.reserve(str.size());

        size_t move {};
        while(move < str.size())
        {
            char32_t out {};
            auto readed = utf8_to_u32(&out, str.data() + move, str.data() + str.size());
            if (readed == 0)
            {
                break;
            }

            move += readed;
            result.push_back(out);
        }

        return result;
    }
}
