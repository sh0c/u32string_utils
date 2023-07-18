#include "u32utils.h"

namespace u32
{
    constexpr char32_t FNT_UNICODE_CODEPOINT_INVALID {0xFFFD};     // Invalid Unicode code point (standard value).
    constexpr char32_t FNT_UNICODE_CODEPOINT_MAX {0x10FFFF};   // Maximum Unicode code point supported by this build.
    // Convert UTF-8 to 32-bits character, process single character input.
    // Based on stb_from_utf8() from github.com/nothings/stb/
    // We handle UTF-8 decoding error by skipping forward.
    size_t text_char_from_utf8(char32_t* out_char, const char* in_text, const char* in_text_end)
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
            *out_char = FNT_UNICODE_CODEPOINT_INVALID; // will be invalid but not end of string
            if(in_text_end && in_text_end - reinterpret_cast<const char*>(str) < 2)
                return 1;
            if(*str < 0xc2)
                return 2;
            c = static_cast<char32_t>((*str++ & 0x1f) << 6);
            if((*str & 0xc0) != 0x80)
                return 2;
            c += (*str++ & 0x3f);
            *out_char = c;
            return 2;
        }
        if((*str & 0xf0) == 0xe0)
        {
            *out_char = FNT_UNICODE_CODEPOINT_INVALID; // will be invalid but not end of string
            if(in_text_end && in_text_end - reinterpret_cast<const char*>(str) < 3)
                return 1;
            if(*str == 0xe0 && (str[1] < 0xa0 || str[1] > 0xbf))
                return 3;
            if(*str == 0xed && str[1] > 0x9f)
                return 3; // str[1] < 0x80 is checked below
            c = static_cast<char32_t>((*str++ & 0x0f) << 12);
            if((*str & 0xc0) != 0x80)
                return 3;
            c += static_cast<char32_t>((*str++ & 0x3f) << 6);
            if((*str & 0xc0) != 0x80)
                return 3;
            c += (*str++ & 0x3f);
            *out_char = c;
            return 3;
        }
        if((*str & 0xf8) == 0xf0)
        {
            *out_char = FNT_UNICODE_CODEPOINT_INVALID; // will be invalid but not end of string
            if(in_text_end && in_text_end - reinterpret_cast<const char*>(str) < 4)
                return 1;
            if(*str > 0xf4)
                return 4;
            if(*str == 0xf0 && (str[1] < 0x90 || str[1] > 0xbf))
                return 4;
            if(*str == 0xf4 && str[1] > 0x8f)
                return 4; // str[1] < 0x80 is checked below
            c = static_cast<char32_t>((*str++ & 0x07) << 18);
            if((*str & 0xc0) != 0x80)
                return 4;
            c += static_cast<char32_t>((*str++ & 0x3f) << 12);
            if((*str & 0xc0) != 0x80)
                return 4;
            c += static_cast<char32_t>((*str++ & 0x3f) << 6);
            if((*str & 0xc0) != 0x80)
                return 4;
            c += (*str++ & 0x3f);
            // utf-8 encodings of values used in surrogate pairs are invalid
            if((c & 0xFFFFF800) == 0xD800)
                return 4;
            // If codepoint does not fit in ImWchar, use replacement character U+FFFD instead
            if (c > FNT_UNICODE_CODEPOINT_MAX) c = FNT_UNICODE_CODEPOINT_INVALID;
            *out_char = c;
            return 4;
        }
        *out_char = 0;
        return 0;
    }

    std::string unicode_to_utf8(const char32_t* in, const char32_t* in_end)
    {
        std::string out;
        char32_t codepoint = 0;
        for (; in != in_end;  ++in)
        {
            if (*in >= 0xd800 && *in <= 0xdbff)
                codepoint = ((*in - 0xd800) << 10) + 0x10000;
            else
            {
                if (*in >= 0xdc00 && *in <= 0xdfff)
                    codepoint |= *in - 0xdc00;
                else
                    codepoint = *in;

                if (codepoint <= 0x7f)
                    out.append(1, static_cast<char>(codepoint));
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

    std::string convert(const std::u32string &str)
    {
        return unicode_to_utf8(str.data(), str.data() + str.size());
    }

    std::u32string convert(const std::string &str)
    {
        std::u32string result {};
        result.reserve(str.size());

        size_t move {};
        while(move < str.size())
        {
            char32_t out {};
            auto readed = text_char_from_utf8(&out, str.data() + move, str.data() + str.size());
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
