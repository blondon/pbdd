#ifndef _PBDD_H
#define _PBDD_H



#include <pthread.h>
#include "bdd.h"
#include "kernel.h"

#define MAX_BDD_OP (bddop_simplify + 1)

typedef long long hash_t;

typedef struct
{
	union
	{
		double dres;
		long long lres;
		int res;
	} r;
	int a, b, c;
	pthread_rwlock_t lock;
} pBddCacheData;

typedef struct
{
	pBddCacheData *table;
	int tablesize;
} pBddCache;

#ifndef APPLYHASH
extern long long APPLYHASH(BDD l, BDD r);
#endif

#ifndef NODEHASH
#define NODEHASH(lvl,l,h)  (TRIPLE(lvl,l,h) % bddnodesize)
#endif

#define pBddCache_LookUp(cache, hash) (&(cache)->table[hash] % (cache)->tablesize;

//extern pBddCacheData * pBddCache_LookUp(pBddCache *, hash_t hash);

extern int pBddCache_init(pBddCache *, int);
extern int pBddCache_done(pBddCache *);
extern int pBddCache_resize(pBddCache *, int);
extern void pBddCache_reset(pBddCache *);

extern pBddCache *cur_cache;
extern pBddCache papplycache[MAX_BDD_OP];

static inline pBddCache *pbdd_get_applycache(int op)
{
	return &papplycache[op];
}

extern int pbdd_operator_init(int cacheSize);
extern void pbdd_operator_done(void);

extern int pbdd_init(int initnodesize, int cachesize);
extern void pbdd_done();
extern BDD pbdd_check_terminal_case(BDD l, BDD r, int applyop);
extern BDD pbdd_apply_rec(BDD l, BDD r, int applyop);
extern BDD pbdd_apply(BDD l, BDD r, int applyop);

extern BDD pbdd_makenode(unsigned int level, BDD low, BDD high, int *creat);

#endif
