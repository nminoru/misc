#define __USE_XOPEN2K8
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <locale.h>

uint32_t covert_unicode(const char* string)
{
    uint32_t a = (uint8_t)string[0];

    if (a < 0x80)
        return a;
    else {
        uint32_t b = (uint8_t)string[1];
        
        if (a < 0xE0) {
            return ((a & 0x1F) << 6) | (b & 0x3F);
        } else {
            uint32_t c = (uint8_t)string[2];

            if (a < 0xF0)
                return ((a & 0xF) << 12) | ((b & 0x3F) << 6) | (c & 0x3F);
            else {
                uint32_t d = (uint8_t)string[3];
                return ((a & 0x7)  << 18) | ((b & 0x3F) << 12) | ((c & 0x3F) << 6) | (d & 0x3F);
            }
        }
    }
}

int covert_utf8(char* dest, uint32_t value)
{
    if (value < 0x80) {
        dest[0] = (char)value;
        return 1;
    } else if (value < 0x800) {
        dest[1] = (char)(0x80 | (value & 0x3F));
        value >>= 6;
        dest[0] = (char)(0xC0 | value);
        return 2;
    } else if (value < 0x10000) {
        dest[2] = (char)(0x80 | (value & 0x3F));
        value >>= 6;
        dest[1] = (char)(0x80 | (value & 0x3F));
        value >>= 6;
        dest[0] = (char)(0xE0 | value);
        return 3;
    } else {
        dest[3] = (char)(0x80 | (value & 0x3F));
        value >>= 6;
        dest[2] = (char)(0x80 | (value & 0x3F));
        value >>= 6;
        dest[1] = (char)(0x80 | (value & 0x3F));
        value >>= 6;
        dest[0] = (char)(0xF0 | value);
        return 4;
    }
}

int main(int argc, char ** argv)
{
    int i, j;
    locale_t mylocale;
    const char *locale;

    // locale_t mylocale = newlocale(LC_ALL_MASK, "en_US.utf8", (locale_t)NULL);
    // locale_t mylocale = newlocale(LC_ALL_MASK, "en_GB.utf8", (locale_t)NULL);
    // locale_t mylocale = newlocale(LC_ALL_MASK, "ja_JP.utf8", (locale_t)NULL);
    // locale_t mylocale = newlocale(LC_ALL_MASK, "zh_TW.utf8", (locale_t)NULL);

#if 1
    // char inbuf[] = "\u00E6";
    char inbuf[16];
    char outbuf[4096];

    if (argc < 3) {
        printf("Usage: locale-name string\n");
        exit(EXIT_SUCCESS);
    }

    locale = argv[1];

    memset(inbuf,  0, sizeof(inbuf));
    memset(outbuf, 0, sizeof(outbuf));

    mylocale = newlocale(LC_ALL_MASK, locale, (locale_t)NULL);
    if (!mylocale) {
        fprintf(stderr, "Not found locale: %s\n", locale);
        exit(EXIT_FAILURE);
    }

#if 1
    strcpy(inbuf, argv[2]);
#else
    inbuf[0] = 'a';
    inbuf[1] = 'e';
    covert_utf8(inbuf + 2, 243);
#endif

    strxfrm_l(outbuf, inbuf, sizeof(outbuf), mylocale);

    printf("INPUT : \"%s\"\n", inbuf);
    printf("OUTPUT:");
    for (j = 0 ; (j < sizeof(outbuf)) && outbuf[j] ; j++)
        printf(" %02x", (uint8_t)outbuf[j]);
    printf("\n");

#else
    for (i=0 ; i<256 ; i++) {
        char inbuf[2];
        char outbuf[16];

        memset(inbuf,  0, sizeof(inbuf));
        memset(outbuf, 0, sizeof(outbuf));

        inbuf[0] = i;
        
        strxfrm_l(outbuf, inbuf, sizeof(outbuf), mylocale);

        uint32_t code = covert_unicode(outbuf);
        printf("%u %u :", i, code);
        for (j=0 ; j<sizeof(outbuf) ; j++)
            printf(" %02x", (uint8_t)outbuf[j]);
        printf("\n");
    }
#endif
        
    return 0;
}
