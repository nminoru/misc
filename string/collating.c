#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <langinfo.h>
#include <locale.h>
#include <xlocale.h>

union locale_data_value
{
    const uint32_t *wstr;
    const char *string;
    unsigned int word;          /* Note endian issues vs 64-bit pointers.  */
};

struct __locale_data
{
    const char *name;
    const char *filedata;         /* Region mapping the file data.  */
    // off_t filesize;               /* Size of the file (and the region).  */
    uint64_t filesize;

    enum                          /* Flavor of storage used for those.  */
    {
        ld_malloced,                /* Both are malloc'd.  */
        ld_mapped,                  /* name is malloc'd, filedata mmap'd */
        ld_archive                  /* Both point into mmap'd archive regions.  */
    } alloc;

    /* This provides a slot for category-specific code to cache data computed
     about this locale.  That code can set a cleanup function to deallocate
     the data.  */
    struct
    {
        void (*cleanup) (struct __locale_data *);
        union
        {
            void *data;
            struct lc_time_data *time;
            const struct gconv_fcts *ctype;
        };
    } private;

    unsigned int usage_count;     /* Counter for users.  */
    
    int use_translit;             /* Nonzero if the mb*towv*() and wc*tomb()
                                     functions should use transliteration.  */

    unsigned int nstrings;        /* Number of strings below.  */

    union locale_data_value values[];
};


int main(int argc, char **argv)
{
    locale_t l;
    struct __locale_data *current;
    uint_fast32_t nrules;
    const unsigned char *rulesets;
    const int32_t *table;
    const unsigned char *weights;
    const unsigned char *extra;
    const int32_t *indirect;
    const char *locale;

    if (argc < 2) {
        printf("Usage: locale-name\n");
        exit(EXIT_SUCCESS);
    }

    locale = argv[1]; 

    l = newlocale(LC_ALL_MASK, locale, (locale_t)NULL);

    if (!l) {
        fprintf(stderr, "Not found locale: %s\n", locale);
        exit(EXIT_FAILURE);
    }

    current = l->__locales[LC_COLLATE];
    nrules = current->values[_NL_ITEM_INDEX(_NL_COLLATE_NRULES)].word;
    rulesets = (const unsigned char *)
        current->values[_NL_ITEM_INDEX(_NL_COLLATE_RULESETS)].string;
    table = (const int32_t *)
        current->values[_NL_ITEM_INDEX(_NL_COLLATE_TABLEMB)].string;
    weights = (const unsigned char *)
        current->values[_NL_ITEM_INDEX(_NL_COLLATE_WEIGHTMB)].string;
    extra = (const unsigned char *)
        current->values[_NL_ITEM_INDEX(_NL_COLLATE_EXTRAMB)].string;
    indirect = (const int32_t *)
        current->values[_NL_ITEM_INDEX(_NL_COLLATE_INDIRECTMB)].string;

    printf("locale name = %s\n", locale);
    printf("nrules = %u\n", nrules);

    uint_fast32_t i;

    for (i=0 ; i<nrules ; i++)
        printf("rulesets[%u] = %u\n", i, rulesets[i]);

    for (i=0 ; i<256 ; i++)
        printf("table[0x%02x] = %d\n", i, table[i]);

#if 0
    for (i=0 ; i<0x1000000 ; i++)
        printf("weights[0x%06x] = %3d\n", i, weights[i]);
#endif

    for (i=0 ; i<nrules ; i++)
        printf("extra[%u] = %u\n", i, extra[i]);

    for (i=0 ; i<nrules ; i++)
        printf("indirect[%u] = %u\n", i, indirect[i]);

    return 0;
}
