#include "prime.h"
#include "pbdd.h"
#include <stdlib.h>

pBddCache *cur_cache;
pBddCache pappplycache[MAX_BDD_OP];


hash_t APPLYHASH(BDD l, BDD r)
{
	hash_t hash;
	if (l > r)
		hash = ((hash_t)r << 32 | l);
	else
		hash = ((hash_t)l << 32 | r);

	return hash;
}

int pBddCache_init(pBddCache *cache, int size)
{
	int n;
	size = bdd_prime_gte(size);
	if ((cache->table = NEW(pBddCacheData,size)) == NULL)
		return bdd_error(BDD_MEMORY);
	
	for (n = 0; n < size; n++)
	{
		cache->table[n].r.lres = INVALID_BDD;
		cache->table[n].a = INVALID_BDD;
		pthread_rwlock_init(&cache->table[n].lock, NULL);
	}
	cache->tablesize = size;
	
	return 0;
}

int pBddCache_done(pBddCache *cache)
{
	free(cache->table);
	cache->table = NULL;
	cache->tablesize = 0;
}

//pBddCacheData * pBddCache_LookUp(pBddCache *cache, hash_t hash) {
//return cache->table[hash] % cache->tablesize;
//}

int pBddCache_resize(pBddCache *cache, int newsize)
{
	int n;
	free(cache->table);
	
	newsize = bdd_prime_gte(newsize);
	if ((cache->table = NEW(pBddCacheData, newsize)) == NULL)
		return bdd_error(BDD_MEMORY);
	
	for (n = 0; n < newsize; n++)
	{
		cache->table[n].r.lres = INVALID_BDD;
		cache->table[n].a = INVALID_BDD;
		pthread_rwlock_init(&cache->table[n].lock, NULL);
	}
	cache->tablesize = newsize;
	
	return 0;
}

int pbdd_operator_init(int cachesize)
{
	int i, j;
	int r;
	for (i = 0; i < MAX_BDD_OP; ++i)
	{
		r = pBddCache_init(&papplycache[i], cachesize);
		if (r < 0)
		{
			for (j = 0; j < i; ++j)
				pBddCache_done(&papplycache[j]);
			return r;
		}
	}

	return 0;
}

void pbdd_operator_done(void)
{
	int i;

	for (i = 0; i < MAX_BDD_OP; ++i)
	{
		pBddCache_done(&papplycache[i]);
	}
}
