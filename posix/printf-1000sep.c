/*
 * thousands separator
 */
#include <stdio.h>
#include <locale.h>

static void printf_thousands_separator(void)
{
    printf("\t%d -> %'d\n",     12345678,      12345678);
    printf("\t%f -> %'f\n",     3.1415926535,  3.1415926535);
    printf("\t%f -> %'f\n",     9999.999,      9999.9999);
    // printf("\t[%'16d]\n", 12345678); // OK
    // printf("\t[%16'd]\n", 12345678); // NG
    printf("\n");
}

int main(int argc, char **argv)
{
    const char *ret;

    ret = setlocale(LC_NUMERIC, "");
    printf("LC_NUMERIC: default %s\n", ret);
    printf_thousands_separator();

    ret = setlocale(LC_NUMERIC, "C");
    printf("LC_NUMERIC: %s\n", ret);
    printf_thousands_separator();

    ret = setlocale(LC_NUMERIC, "en_US");
    printf("LC_NUMERIC: %s\n", ret);
    printf_thousands_separator();

    ret = setlocale(LC_NUMERIC, "ja_JP");
    printf("LC_NUMERIC: %s\n", ret);
    printf_thousands_separator();

    return 0;
}
