#ifndef _PBDD_H
#define _PBDD_H


#ifdef CPLUSPLUS
extern "C" {
#endif

#define MAX_BDD_OP (bddop_simplify + 1)
#define MAX_BDD_SYMM_OP (bddop_nor + 1)

typedef long long hast_t;

extern int applyop;
extern int pbdd_init(int initnodesize, int cachesize);

#ifdef CPLUSPLUCS
}
#endif

#endif