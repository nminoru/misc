#include <stdio.h>
#include <locale.h>
#include <langinfo.h>

int main(int argc, char **argv)
{
    setlocale(LC_COLLATE, "ja_JP.UTF-8");

    printf("_NL_COLLATE_NRULES           = %p\n", nl_langinfo(_NL_COLLATE_NRULES));
    printf("_NL_COLLATE_RULESETS         = %p\n", nl_langinfo(_NL_COLLATE_RULESETS));
    printf("_NL_COLLATE_TABLEMB          = %p\n", nl_langinfo(_NL_COLLATE_TABLEMB));
    printf("_NL_COLLATE_WEIGHTMB         = %p\n", nl_langinfo(_NL_COLLATE_WEIGHTMB));
    printf("_NL_COLLATE_EXTRAMB          = %p\n", nl_langinfo(_NL_COLLATE_EXTRAMB));
    printf("_NL_COLLATE_INDIRECTMB       = %p\n", nl_langinfo(_NL_COLLATE_INDIRECTMB));
    printf("_NL_COLLATE_GAP1             = %p\n", nl_langinfo(_NL_COLLATE_GAP1));
    printf("_NL_COLLATE_GAP2             = %p\n", nl_langinfo(_NL_COLLATE_GAP2));
    printf("_NL_COLLATE_GAP3             = %p\n", nl_langinfo(_NL_COLLATE_GAP3));
    printf("_NL_COLLATE_TABLEWC          = %p\n", nl_langinfo(_NL_COLLATE_TABLEWC));
    printf("_NL_COLLATE_WEIGHTWC         = %p\n", nl_langinfo(_NL_COLLATE_WEIGHTWC));
    printf("_NL_COLLATE_EXTRAWC          = %p\n", nl_langinfo(_NL_COLLATE_EXTRAWC));
    printf("_NL_COLLATE_INDIRECTWC       = %p\n", nl_langinfo(_NL_COLLATE_INDIRECTWC));
    printf("_NL_COLLATE_SYMB_HASH_SIZEMB = %p\n", nl_langinfo(_NL_COLLATE_SYMB_HASH_SIZEMB));
    printf("_NL_COLLATE_SYMB_TABLEMB     = %p\n", nl_langinfo(_NL_COLLATE_SYMB_TABLEMB));
    printf("_NL_COLLATE_SYMB_EXTRAMB     = %p\n", nl_langinfo(_NL_COLLATE_SYMB_EXTRAMB));
    printf("_NL_COLLATE_COLLSEQMB        = %p\n", nl_langinfo(_NL_COLLATE_COLLSEQMB));
    printf("_NL_COLLATE_COLLSEQWC        = %p\n", nl_langinfo(_NL_COLLATE_COLLSEQWC));
    printf("_NL_COLLATE_CODESET          = %p\n", nl_langinfo(_NL_COLLATE_CODESET));

    return 0;
}

