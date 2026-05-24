# u32string_utils

A C++14 header-based library providing `char32_t` / `std::u32string` equivalents of standard library functions — without any dependency on the standard localization (`std::locale`) library. Includes numeric conversions, UTF-8/UTF-16 ↔ UTF-32 encoding helpers, Unicode character classification/case conversion, and legacy charset transcoding.

---

## Table of Contents

- [Features](#features)
- [Requirements](#requirements)
- [Building & Installation](#building--installation)
- [Modules](#modules)
  - [u32strlib — Numeric Conversions](#u32strlib--numeric-conversions)
  - [u32utils — String Encoding Conversion](#u32utils--string-encoding-conversion)
  - [u32caps — Character Classification & Case](#u32caps--character-classification--case)
  - [u32charset — Legacy Charset Transcoding](#u32charset--legacy-charset-transcoding)
- [License](#license)

---

## Features

- **Numeric parsing** — `atoi`, `atol`, `atoll`, `strtoi`, `strtol`, `strtoll`, `strtoul`, `strtoull`, `stoi`, `stol`, `stoll`, `stoul`, `stoull`, `atof`, `strtof`, `strtod`, `strtold`, `stof`, `stod`, `stold` — all operating on `char32_t*` / `std::u32string`.
- **Encoding conversion** — High-level helpers and low-level char-by-char converters between UTF-8, UTF-16, and UTF-32.
- **Unicode character utilities** — `toupper`, `tolower`, `is_upper`, `is_lower`, `is_alpha`, `is_digit`, `is_space`, `is_punctuation`, `is_other_symbol`, `is_currency_symbol` — locale-free, Unicode-aware.
- **Legacy charset support** — Convert between `std::u32string` and 25+ legacy encodings (ISO-8859-x, Windows-125x, KOI8-R, Mac OS Roman, and more).

---

## Requirements

- C++14 or later
- CMake ≥ 3.4.3

---

## Building & Installation

```bash
git clone https://github.com/sh0c/u32string_utils.git
cd u32string_utils
mkdir build && cd build
cmake ..
cmake --build .
```

To include the library in your own CMake project, add it as a subdirectory or install it and then link against `u32string_utils`:

```cmake
target_link_libraries(your_target PRIVATE u32string_utils)
```

The public headers are under the `u32string_utils/` directory and are automatically added to your include path.

---

## Modules

### `u32strlib` — Numeric Conversions

**Header:** `<u32string_utils/u32strlib.h>`

Drop-in equivalents of `<cstdlib>` / `<string>` numeric functions, accepting `char32_t*` or `std::u32string` instead of `char*` / `std::string`. All functions follow the same rules as their standard counterparts — see [cppreference](https://en.cppreference.com/w/cpp/string/byte/atoi) for detailed semantics.

**Supported functions:**

| Category        | Functions |
|-----------------|-----------|
| Integral (C-style)   | `atoi`, `atol`, `atoll`, `strtoi`, `strtol`, `strtoll`, `strtoul`, `strtoull` |
| Integral (C++-style) | `stoi`, `stol`, `stoll`, `stoul`, `stoull` |
| Floating point (C-style)   | `atof`, `strtof`, `strtod`, `strtold` |
| Floating point (C++-style) | `stof`, `stod`, `stold` |

**Example:**

```cpp
#include <iostream>
#include <u32string_utils/u32strlib.h>

int main()
{
    constexpr const char32_t* number_str = U"12345";
    auto number = u32::atoi(number_str);
    std::cout << "Number is: " << number << std::endl;  // Number is: 12345
    return 0;
}
```

---

### `u32utils` — String Encoding Conversion

**Header:** `<u32string_utils/u32utils.h>`

Converts strings between UTF-8, UTF-16, and UTF-32 encodings.

#### High-level conversion

```cpp
#include <u32string_utils/u32utils.h>

// UTF-8 → UTF-32
std::string utf8_string = u8"Това събщение е на кирилица!";
std::u32string u32_string = u32::convert(utf8_string);

// UTF-32 → UTF-8
std::string utf8_back = u32::convert(u32_string);
```

#### Low-level char-by-char conversion

Use `u32::utf8_to_u32` when you need fine-grained control, e.g. for streaming or error handling:

```cpp
#include <u32string_utils/u32utils.h>

std::string utf8_string = u8"Това събщение е на кирилица!";

std::u32string out;
out.reserve(utf8_string.size());

for (size_t i = 0; i < utf8_string.size(); )
{
    char32_t out_char{};
    size_t consumed = u32::utf8_to_u32(&out_char,
                                        utf8_string.data() + i,
                                        utf8_string.data() + utf8_string.size());

    if (consumed == u32::get_unicode_invalid_codepoint_char_id())
        break;

    out.append(1, out_char);
    i += consumed;
}
```

---

### `u32caps` — Character Classification & Case

**Header:** `<u32string_utils/u32caps.h>`

Locale-free Unicode character classification and case conversion. Based on Unicode character categories — see the [Unicode category reference](https://www.compart.com/en/unicode/category).

**Supported functions:**

| Function | Description |
|----------|-------------|
| `toupper(c)` | Convert `char32_t` to uppercase |
| `tolower(c)` | Convert `char32_t` to lowercase |
| `is_upper(c)` | True if uppercase letter |
| `is_lower(c)` | True if lowercase letter |
| `is_alpha(c)` | True if letter (any script) |
| `is_digit(c)` | True if decimal digit |
| `is_space(c)` | True if whitespace |
| `is_punctuation(c)` | True if punctuation character |
| `is_other_symbol(c)` | True if other symbol |
| `is_currency_symbol(c)` | True if currency symbol |

**Example:**

```cpp
#include <u32string_utils/u32caps.h>

char32_t c = U'а';          // Cyrillic small letter a
char32_t upper = u32::toupper(c);  // → U'А'
bool alpha = u32::is_alpha(c);     // → true
```

---

### `u32charset` — Legacy Charset Transcoding

**Header:** `<u32string_utils/u32charset.h>`

Converts between `std::u32string` and legacy single-byte encodings.

**Supported charsets:**

```cpp
enum class charset_t
{
    US_ASCII,
    ISO_8859_1,  ISO_8859_2,  ISO_8859_3,  ISO_8859_4,
    ISO_8859_5,  ISO_8859_6,  ISO_8859_7,  ISO_8859_8,
    ISO_8859_9,  ISO_8859_10, ISO_8859_13, ISO_8859_15,
    WINDOWS_1250, WINDOWS_1251, WINDOWS_1252, WINDOWS_1253,
    WINDOWS_1254, WINDOWS_1255, WINDOWS_1256, WINDOWS_1257,
    WINDOWS_1258,
    ISO_SYMBOL,
    KOI8_R,
    MAC_OS_ROMAN,
    UNSUPPORTED_CHARSET
};
```

**Example — convert a Windows-1251 (Cyrillic) string to UTF-32:**

```cpp
#include <u32string_utils/u32charset.h>

int main()
{
    std::string cp1251_str = "...";  // raw Windows-1251 bytes
    std::u32string u32_str = u32::convert(cp1251_str, u32::charset_t::WINDOWS_1251);
    return 0;
}
```

---

## License

See [LICENSE](LICENSE) in the repository root.
