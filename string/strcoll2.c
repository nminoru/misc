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

int main(int argc, char ** argv)
{
    int i, ret;
    const char *str1;
    const char *str2;
    locale_t mylocale;
    const char *locale;

    // locale_t mylocale = newlocale(LC_ALL_MASK, "ja_JP.utf8", (locale_t)NULL);
    // locale_t mylocale = newlocale(LC_ALL_MASK, "zh_TW.utf8", (locale_t)NULL);
    
    // U+E383BC
    //

    if (argc < 4) {
        printf("Usage: locale-name str1 str2\n");
        exit(EXIT_SUCCESS);
    }

    locale = argv[1];
    str1   = argv[2];
    str2   = argv[3];

    mylocale = newlocale(LC_ALL_MASK, locale, (locale_t)NULL);
    if (!mylocale) {
        fprintf(stderr, "Not found locale: %s\n", locale);
        exit(EXIT_FAILURE);
    }

    ret = strcoll_l(str1, str2, mylocale);

    // printf("%x\n", covert_unicode(str1));
    // printf("%x\n", covert_unicode(str2));

    printf("string compare: \"%s\" %s \"%s\" in %s\n",
           str1,
           ((ret == 0) ? "==" : ((ret > 0) ? ">" : "<")),
           str2,
           locale);
        
    return 0;
}
