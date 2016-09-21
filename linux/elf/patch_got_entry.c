#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <link.h>
#include <elf.h>
#include <sys/mman.h>

#include "patch_got_entry.h"

typedef struct
{
    const char   *symname;
    void         *new_symaddr;
} patch_got_cb_t;

struct strtab
{
    char          *tab;
    ElfW(Xword)    size;
};

struct jmpreltab
{
    ElfW(Rela)    *tab;
    ElfW(Xword)    size;
};


struct symtab
{
    ElfW(Sym)     *tab;
    ElfW(Xword)    entsz;
};

static int callback(struct dl_phdr_info *info, size_t size, void *data);
static void patch_got(ElfW(Addr) base, const ElfW(Phdr) *phdr, ElfW(Half) phnum, const char *symname, void *new_symaddr);
static const ElfW(Phdr) *get_phdr_dynamic(const ElfW(Phdr) *phdr, ElfW(Half) phnum);
static const ElfW(Dyn) *get_dynentry(ElfW(Addr) base, const ElfW(Phdr) *pdyn, uint32_t type);
static struct jmpreltab get_jmprel(ElfW(Addr) base, const ElfW(Phdr) *pdyn);
static struct symtab get_symtab(ElfW(Addr) base, const ElfW(Phdr) *pdyn);
static struct strtab get_strtab(ElfW(Addr) base, const ElfW(Phdr) *pdyn);
static void *get_got_entry(ElfW(Addr) base, struct jmpreltab jmprel, struct symtab symtab, struct strtab strtab, const char *symname);
static const char *get_segment_type(ElfW(Word) type);

void
patch_got_entry(const char *symname, void *new_symaddr)
{
    patch_got_cb_t data;

    data.symname = symname;
    data.new_symaddr = new_symaddr;

    dl_iterate_phdr(callback, &data);
}

static int
callback(struct dl_phdr_info *info, size_t size, void *d)
{
    int i;
    patch_got_cb_t *data = (patch_got_cb_t *) d;

    patch_got(info->dlpi_addr, info->dlpi_phdr, info->dlpi_phnum, data->symname, data->new_symaddr);

    return 0;
}

static void
patch_got(ElfW(Addr) base, const ElfW(Phdr) *phdr, ElfW(Half) phnum, const char *symname, void *new_symaddr)
{
    const ElfW(Phdr) *dphdr;
    struct jmpreltab jmprel;
    struct symtab symtab;
    struct strtab strtab;
    void **targetgot;

    dphdr = get_phdr_dynamic(phdr, phnum);
    jmprel = get_jmprel(base, dphdr);
    symtab = get_symtab(base, dphdr);
    strtab = get_strtab(base, dphdr);

    targetgot = get_got_entry(base, jmprel, symtab, strtab, symname);

    if (targetgot != NULL)
    {
        void *page_addr;
        
        unsigned long pagesize;

        pagesize = (unsigned long) sysconf(_SC_PAGE_SIZE);

        page_addr = (void *)(~(pagesize - 1) & (uintptr_t) targetgot);

        mprotect(page_addr, pagesize, PROT_READ|PROT_WRITE);

        *targetgot = new_symaddr;
    }
}

static const ElfW(Phdr) *
get_phdr_dynamic(const ElfW(Phdr) *phdr, ElfW(Half) phnum)
{
    int i;

    for (i = 0; i < phnum; i++)
        if (phdr[i].p_type == PT_DYNAMIC)
            return phdr + i;

    return NULL;
}

static const ElfW(Dyn) *
get_dynentry(ElfW(Addr) base, const ElfW(Phdr) *pdyn, uint32_t type)
{
    ElfW(Dyn) *dyn;

    for (dyn = (ElfW(Dyn) *)(base + pdyn->p_vaddr); dyn->d_tag; dyn++)
        if (dyn->d_tag == type)
            return dyn;

    return NULL;
}

static struct jmpreltab
get_jmprel(ElfW(Addr) base, const ElfW(Phdr) *pdyn)
{
    struct jmpreltab table = {0};
    const ElfW(Dyn) *dyn;

    if ((dyn = get_dynentry(base, pdyn, DT_JMPREL)) != NULL)
        table.tab = (ElfW(Rela) *) dyn->d_un.d_ptr;

    if ((dyn = get_dynentry(base, pdyn, DT_PLTRELSZ)) != NULL)
        table.size = dyn->d_un.d_val;

    return table;
}

static struct symtab
get_symtab(ElfW(Addr) base, const ElfW(Phdr) *pdyn)
{
    struct symtab table = {0};
    const ElfW(Dyn) *dyn;

    if ((dyn = get_dynentry(base, pdyn, DT_SYMTAB)) != NULL)
        table.tab = (ElfW(Sym) *) dyn->d_un.d_ptr;

    if ((dyn = get_dynentry(base, pdyn, DT_SYMENT)) != NULL)
        table.entsz = dyn->d_un.d_val;

    return table;
}

static struct strtab
get_strtab(ElfW(Addr) base, const ElfW(Phdr) *pdyn)
{
    struct strtab table = {0};
    const ElfW(Dyn) *dyn;

    if ((dyn = get_dynentry(base, pdyn, DT_STRTAB)) != NULL)
        table.tab = (char *) dyn->d_un.d_ptr;

    if ((dyn = get_dynentry(base, pdyn, DT_STRSZ)) != NULL)
        table.size = dyn->d_un.d_val;

    return table;
}

static void *
get_got_entry(ElfW(Addr) base, struct jmpreltab jmprel,
              struct symtab symtab, struct strtab strtab, const char *symname)
{
    ElfW(Rela) *rela;
    ElfW(Rela) *relaend;

    relaend = (ElfW(Rela) *)((char *)jmprel.tab + jmprel.size);

    for (rela = jmprel.tab; rela < relaend; rela++)
    {
        uint32_t relsymidx;
        char *relsymname;

        relsymidx = ELF64_R_SYM(rela->r_info);
        relsymname = strtab.tab + symtab.tab[relsymidx].st_name;
        
        if (strcmp(symname, relsymname) == 0)
            return (void *)(base + rela->r_offset);
    }

    return NULL;
}

static const char *
get_segment_type(ElfW(Word) type)
{
    switch (type)
    {
    case PT_NULL:
        return "PT_NULL";
    case PT_LOAD:
        return "PT_LOAD";
    case PT_DYNAMIC:
        return "PT_DYNAMIC";
    case PT_INTERP:
        return "PT_INTERP";
    case PT_NOTE:
        return "PT_NOTE";
    case PT_SHLIB:
        return "PT_SHLIB";
    case PT_PHDR:
        return "PT_PHDR";
    case PT_TLS:
        return "PT_TLS";
    case PT_NUM:
        return "PT_NUM";
    case PT_LOOS:
        return "PT_LOOS";
    case PT_GNU_EH_FRAME:
        return "PT_GNU_EH_FRAME";
    case PT_GNU_STACK:
        return "PT_GNU_STACK";
    case PT_GNU_RELRO:
        return "PT_GNU_RELRO";
    case PT_HIOS:
        return "PT_HIOS";
    case PT_LOPROC:
        return "PT_LOPROC";
    case PT_HIPROC:
        return "PT_HIPROC";
    default:
        return NULL;
    }
}
