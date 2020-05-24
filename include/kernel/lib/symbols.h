#ifndef __SYMBOLS_H__
#define __SYMBOLS_H__

#include <kernel/multiboot.h>

#define SYM_NAME_SIZ 64

size_t parse_symlist(MultibootModule *syms, char sym_name[SYM_NAME_SIZ], size_t addr);

#endif /* __SYMBOLS_H__ */
