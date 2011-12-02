#ifndef _PBDDCACHE_H
#define _PBDDCACHE_H

#include "pbdd.h"

#ifdef CPLUSPLUS
extern "C" {
#endif



typedef struct
{
	union
	{
		double dres;
		long long lres;
		int res;
	} r;
	int a, b, c;
	pthread_wlock_t lock;
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

#ifdef CPLUSCPLUS
}
#endif

#endif
