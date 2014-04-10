#include "bin_pack.h"
#include "bin_pack_endian.h"
#include "bin_pack_debug.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>         
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

/*
 * put the test code here
 */

static void test_pack_string()
{
    const char ptn[] = "You jump, I jump.";
    printf ("test pack string\n");

    smart_str str = {0};
    bin_packer_t pk;
    bin_packer_init(&pk, smart_write, &str, 0);

    int ret = bin_pack_string(&pk, (const char *)&ptn);
    print_str(str.c, str.len);

    bin_unpacker_t uk;
    bin_unpacker_init(&uk, str.c, str.len, 0);                                                              \

    char *p;
    size_t len = 0;
    bin_unpack_lstring(&uk, &p, &len);
    printf("%s\n", ptn);
    printf("%s\n", p);
}

int main ()
{
    printf ("test pack integer\n");
    intmax_t i1 = 1;
    test(bin_pack_integer, bin_unpack_integer, intmax_t, i1);

    bool b1 = true;
    printf ("test pack bool\n");
    test(bin_pack_bool, bin_unpack_bool, bool, b1);

    printf ("test pack float\n");
    float f1 = 1.1;
    test(bin_pack_real_float, bin_unpack_real_float, float, f1);

    printf ("test pack double\n");
    double d1 = 3.15;
    test(bin_pack_real_double, bin_unpack_real_double, double, d1);

    test_pack_string();
}
