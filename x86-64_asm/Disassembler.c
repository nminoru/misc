/**
 *  gcc disassembler.c -lopcodes -lbfd -liberty
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <bfd.h>
#include <dis-asm.h>


/* */
static char buffer[256];

static int read_memory_func(bfd_vma memaddr, bfd_byte *myaddr, unsigned int length, struct disassemble_info *info);

typedef struct {
    char *buffer;
    char *current;
} SFILE;

static int
objdump_sprintf (SFILE *f, const char *format, ...)
{
    int n;
    va_list args;

    va_start (args, format);
    vsprintf (f->current, format, args);
    f->current += n = strlen (f->current);
    va_end (args);

    return n;
}


/**
 *  start_memaddr から length バイト分の命令を逆アセンブラした結果を
 *  stream に出力する。
 */
extern int 
disassemble_instructions(const char* start_memaddr, int length, FILE* stream)
{
    disassemble_info  dinfo;

    bfd_vma start, end, eip;

    bfd_init();

    SFILE sfile;

    // INIT_DISASSEMBLE_INFO(dinfo, stdout, (fprintf_ftype) fprintf);
    INIT_DISASSEMBLE_INFO(dinfo, (SFILE*)&sfile, (fprintf_ftype) objdump_sprintf);
  
    dinfo.read_memory_func = read_memory_func;

    start = (bfd_vma) start_memaddr;
    end   = (bfd_vma) start + length;

#if defined(__i386__)
    dinfo.mach = bfd_mach_i386_i386;
#elif defined(__x86_64__)
    dinfo.mach = bfd_mach_x86_64;
#endif

    // dinfo.bytes_per_line = 0;
    // dinfo.bytes_per_chunk = 0;

    for (eip = start; eip < end ;) {
        int i, ret, sum;
        unsigned char* p = (unsigned char*) eip;

        sfile.buffer  = buffer;
        sfile.current = buffer;

#if defined(__ia64__)			
        fprintf(stream, "%p %02x %02x %02x %02x %02x %02x : ",
                p, p[0], p[1], p[2], p[3], p[4], p[5]);
        fflush(stream);
        ret = print_insn_ia64(eip, &dinfo);
        buffer[ret] = '\0';
        fputs(buffer, stream);
    
        fflush(stream);
        fprintf(stream, " \n");
        fflush(stream);

#elif defined(__i386__) || defined(__x86_64__) 
        fprintf(stream, "%p : ", p);
        fflush(stream);

        // ret = print_insn_i386(eip, &dinfo);
        ret = print_insn_i386_att(eip, &dinfo);
        // ret = print_insn_i386_intel(eip, &dinfo);


        sum = 0;
        for (i=0 ; i<ret ; i++) {
            sum += fprintf(stream, "%02x ", *((unsigned char*)eip + i)); 
        }
        fprintf(stream, "%*s", 36 - sum, "");

        fputs(buffer, stream);

        fprintf(stream, " \n");
        fflush(stream);
#else
#error "Not supported for this platform."
#endif

        if (ret <= 0) {
            break;
        } else {
            eip += ret;
        }
    }

    return 0;
}


static int read_memory_func(bfd_vma memaddr, bfd_byte *myaddr,
			    unsigned int length,
                            struct disassemble_info *info)
{
    int i;

    for (i=0 ; i<length; i++) {
        myaddr[i] = ((char*)memaddr)[i];
    }

    return 0;
}
