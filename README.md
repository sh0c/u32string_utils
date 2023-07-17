# char32_t* and std::u32string functions that converts string to int/float and etc.

## This implementation uses standard library rules for this functions:

## strtoi(), strtol(), strtoll(), strtoul(), strtoull():

Parses the C-string **(const char32_t*) str** interpreting its content as an integral number of the specified base, which is returned as a specific function type(T) value. If **endptr** is not a null pointer, the function also sets the value of **endptr** to point of the first character after the number.

The function (*strtoi(), strtol(), strtoll(), strtoul() or strtoull()*) discards as many whitespace characters as necessary until the first non-whitespace character is found. Then, starting from this character, the function takes as many characters (valid against the **base** parameter syntax) as possible, and interprets them as a numerical value. Finally, a pointer to the first character, following the integer representation in **str**, is stored in the object pointed by **endptr**.

If the value of **base** is zero, the expected syntax is similar to that of integer constants, which is formed by a succession of:
-An optional sign character (+ or -)
-An optional prefix indicating *octal* or *hexadecimal* base ("0" or "0x"/"0X" respectively)
-A sequence of *decimal* digits (if no base prefix was specified) either *octal* or *hexadecimal* digits if a specific prefix is present

If the **base** value is between 2 and 36, the expected format for the integral number is a succession of valid digits and/or letters needed to represent integers of the specified radix (starting from '0' and up to 'z'/'Z' for radix 36).

If the first sequence of non-whitespace characters in **str** is not a valid integral number as defined above, or if no such sequence exists because either **str** is empty or it contains only whitespace characters, no conversion is performed.

### Parameters
```
str - const char32_t* string with the representation of an integral number.
endptr - Reference to an object of type char32_t*, whose value is set by the function to the next character in str after the numerical value. This parameter can also be a null pointer and in this case it is not used.
base - Numerical base (radix) that determines the valid characters and their interpretation. If base is 0, the used radix is determined by the format in the sequence (see above).
```

### Return Value
On success, the function returns the converted integral number as a type(T) of value for specific function (strtoi() - int, strtol() - long, strtoll() - long long, strtoul() - unsigned long, strtoull() - unsigned long long).
If no valid conversion could be performed, a zero value is returned (0).
If the read value is out of the range of representable values by a specific function type(T), the function returns *std::numeric_limits<T>::max()* or *std::numeric_limits<T>::min()* (defined in <numeric_limits>), and **errno** is set to *ERANGE*.

## atoi(), atol(), atoll():
These functions are strtoi() (for atoi()), strtol() (for atol()) and strtoll() (for atoll()) with *null pointer* for **endptr** and **base** = 10. The implementation is:
```
int atoi(const char32_t* str)
{
    return strtoi(str, nullptr, 10);
}
```

