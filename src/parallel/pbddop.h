#ifndef _PBDDOP_H
#define _PBDDOP_H

#include "pbdd.h"

#ifdef CPLUSPLUS
extern "C" {
#endif

extern int pbdd_init(int initnodesize, int cachesize);
extern int pbdd_done();
extern BDD pbdd_check_terminal_case(BDD l, BDD r, int applyop);
extern BDD pbdd_apply_rec(BDD l, BDD r, int applyop);
extern BDD pbdd_apply(BDD l, BDD r, int applyop);

extern BDD pbdd_makenode(unsigned int level, BDD low, BDD high, int *creat);

#ifdef CPLUSPLUS
}
#endif

#endif 
