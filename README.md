# char32_t implementation of standard library functions

[![stable](http://badges.github.io/stability-badges/dist/stable.svg)](http://github.com/badges/stability-badges)

## This implementation uses standard library rules for this functions and does not use std localization library. Supported functions in u32strlib.h:
### Integral: atoi(), atol(), atoll(), strtoi(), strtol(), strtoll(), strtoul(), strtoull(),
###           stoi(), stol(), stoll(), stoul(), stoull();
### Floating point: atof(), strtof(), strtod(), strtold(),
###                 stof(), stod(), stold();

Usage can be found [here](https://en.cppreference.com/w/cpp/string/byte/atoi) for all functions! Difference is that this library uses char32_t* for string type.

### Example:
```
#include <iostream>
#include <string>
#include <u32string_utils/u32strlib.h>

int main()
{
    constexpr const char32_t* number_str {U"12345"};
    auto number = u32::atoi(number_str);
    std::cout << "Number is: " << number << std::endl;

    return 0;
}
```

## Convert strings between utf8 <-> utf32, utf16 <-> utf32
### Using helper functions users can convert string in u32utils.h
### Example:

Convert from utf8 string to u32 string and back:
```
    std::string utf8_string = u8"Това събщение е на кирилица!";
    std::u32string u32_string = u32::convert(utf8_string);
    std::sring utf8_string_back = u32::convert(u32_string);
```

Provide char*, char16_t*, char32_t* convert:
```
    std::string utf8_string = u8"Това събщение е на кирилица!";

    std::u32string out {};
    out.reserve(utf8_string.size())
    for (size_t i = 0; i < utf8_string.size(); ) {
        char32_t out_char {};
        size_t consumed = u32::utf8_to_u32(&char, utf8_string.data() + i, utf8_string.data() + utf8_string.size());
        if (consumed == u32::get_unicode_invalid_codepoint_char_id())
        {
            break;
        }

        out.append(1, out_char);
        i += consumed;
    }
```

## Find char32_t symbols are in specific character category, convert char32_t symbol to upper or to lower without using standard localization library:
### Supported functions in u32caps.h:
### toupper(), tolower(), is_upper(), is_lower(), is_alpha(), is_digit(), is_space(), is_punctuation(), is_other_symbol(), is_currency_symbol()

Information about Unicode characters group can be found [here](https://www.compart.com/en/unicode/category)
