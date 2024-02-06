#pragma once

#include <string>

namespace u32
{
    enum class charset_t
    {
        US_ASCII,
        ISO_8859_1,
        ISO_8859_2,
        ISO_8859_3,
        ISO_8859_4,
        ISO_8859_5,
        ISO_8859_6,
        ISO_8859_7,
        ISO_8859_8,
        ISO_8859_9,
        ISO_8859_10,
        ISO_8859_13,
        ISO_8859_15,
        WINDOWS_1250,
        WINDOWS_1251,
        WINDOWS_1252,
        WINDOWS_1253,
        WINDOWS_1254,
        WINDOWS_1255,
        WINDOWS_1256,
        WINDOWS_1257,
        WINDOWS_1258,
        ISO_SYMBOL,
        KOI8_R,
        MAC_OS_ROMAN,
        UNSUPPORTED_CHARSET
    };

    char32_t charset_to_utf32(char ch, charset_t encoding);
    char utf32_to_charset(char32_t ch, charset_t encoding);

    std::u32string convert(const std::string& str, charset_t encoding);
    std::string convert(const std::u32string& str, charset_t encoding);
}
