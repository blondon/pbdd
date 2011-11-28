#include "../prime.h"
#include "pbddop.h"

BddCache *cur_cache;
BddCache pappplycache[MAX_BDD_OP];


hash_t APPLYHASH(BDD l, BDD r, int op)
{
	hast_t hash;
	if ((apply_symm == 1) && (l > r))
		hash = ((hash_t)r << 32 | l);
	else
		hash = ((hash_t)r << 32 | r);

	return hash;
}

int BddCache_init(BddCache *cache, int size)
{
	int n;
	size = bdd_prime_gte(size);
	if ((cache->table = NEW(BddCacheData,size)) == NULL)
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

int BddCache_done(BddCache *cache)
{
	free(cache->table);
	cache->table = NULL;
	cache->tablesize = 0;
}

int BddCache_resize(BddCache *cache, int newsize)
{
	int n;
	free(cache->table);
	
	newsize = bdd_prime_gte(newsize);
	if ((cache->table = NEW(BddCacheData, newsize)) == NULL)
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
		r = BddCache_init(&papplycache[i], cachesize);
		if (r < 0)
		{
			for (j = 0; j < i; ++j)
				BddCache_done(&papplycache[j]);
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
		BddCache_done(&papplycache[i]);
	}
}
