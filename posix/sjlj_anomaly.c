#include <stdio.h>
#include <setjmp.h>

extern void throw_exception(void) __attribute__((noinline));

sigjmp_buf env;

int main(int argc, char **argv)
{
    int count1 = 0;
    volatile count2 = 0;

    if (sigsetjmp(env, 1) == 0)
    {
        /* try 句 にあたる部分 */
        int i;

        printf("TRY\n");

        for (i =  0 ; i < 1000 ; i++)
        {
            count1 = i;
            count2 = i;

            if (i == 50)
                throw_exception();
        }
    }
    else
    {
        /* catch 句にあたる部分 */
        printf("CATCH\n\n");
        printf("  count1 = %d\n", count1);
        printf("  count2 = %d\n", count2);
    }

    return 0;
}

void
throw_exception(void)
{
    siglongjmp(env, 1);
}
