#include <u32string_utils/u32strlib.h>

#include <cassert>

int main()
{
    {
        auto result = u32::atoi(U"12345678");
        assert(result == 12345678);
    }

    return 0;
}
