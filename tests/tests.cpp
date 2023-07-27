#include "atoX_integral.h"
#include "stoX_integral.h"
#include "strtoX_integral.h"

#include "atoX_real.h"

int main()
{
    // check atoX
    check_atoX_integral<int>(u32::atoi);
    check_atoX_integral<long>(u32::atol);
    check_atoX_integral<long long>(u32::atoll);

    // check strtoX
    check_strtoX<int>(u32::strtoi);
    check_strtoX<long>(u32::strtol);
    check_strtoX<long long>(u32::strtoll);

    // check strtouX
    check_strtoX<unsigned long>(u32::strtoul);
    check_strtoX<unsigned long long>(u32::strtoull);

    // check stoX
    check_stoX<int>(u32::stoi);
    check_stoX<long>(u32::stol);
    check_stoX<long long>(u32::stoll);

    // check stouX
    check_stoX<unsigned long>(u32::stoul);
    check_stoX<unsigned long long>(u32::stoull);

    // check atoX
    check_atoX_real<double>(u32::atof);

    return 0;
}
