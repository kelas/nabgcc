// VLISP Virtual Machine - 2006 - by Sylvain Huet
// Lowcost IS Powerfull

#ifndef _LOADER_
#define _LOADER_

#define SYS_NB 6
#define SYS_CBPLAY 0
#define SYS_CBUDP 1
#define SYS_CBTCP 2
#define SYS_CBLOOP 3
#define SYS_ENV 4
#define SYS_CBREC 5

#define bytecode ((uint8_t*)vmem_heap)
//extern char bytecode[65536];
//extern uint8_t *bytecode;


extern uint8_t *bc_tabfun;
extern int32_t bc_nbfun;
extern int32_t sys_start;
extern int32_t global_start;

int32_t loaderGetInt(uint8_t *src);
int32_t loaderGetShort(uint8_t *src);
int32_t loaderFunstart(int32_t funnumber);
int32_t loaderInit(uint8_t *src);

#endif
