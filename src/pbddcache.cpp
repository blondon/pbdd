#include "pbdd.h"
#include <stdlib.h>
#include <assert.h>

pBddCache *cur_cache;
pBddCache papplycache[MAX_BDD_OP];


hash_t APPLYHASH( int l,  int r)
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
	if ((cache->table = NEW(pBddCacheData,size)) == NULL)
		exit(-1);
	
	for (n = 0; n < size; n++)
	{
		cache->table[n].res = NULL;
        cache->table[n].invalid = TRUE;
		cache->table[n].lock = FALSE;
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

int pBddCache_resize(pBddCache *cache, int newsize)
{
	int n;
	free(cache->table);
	if ((cache->table = NEW(pBddCacheData, newsize)) == NULL)
		exit(-1);
	
	for (n = 0; n < newsize; n++)
	{
		cache->table[n].res = NULL;
        cache->table[n].invalid = TRUE;
		cache->table[n].lock = FALSE;
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

void pbdd_operator_done()
{
	int i;

	for (i = 0; i < MAX_BDD_OP; ++i)
	{
		pBddCache_done(&papplycache[i]);
	}
}

BddNode* pBddCache_read(pBddCacheData * entry,  int l,  int r) {
  BddNode* res = NULL;

  //acquire the lock
  while(__sync_val_compare_and_swap(&entry->lock,FALSE,TRUE) == TRUE) 
  {
    //spin while you didn't acquire
  }
  //make sure you have the lock
  assert(entry->lock == TRUE);

  //read the cache
  if (entry->a == l && entry->b == r)
  {
    assert(entry->invalid != TRUE);
    res = entry->res;
  }
  //release the lock
  assert(__sync_val_compare_and_swap(&entry->lock,TRUE,FALSE) == TRUE);
  return res;
}

void pBddCache_insert(pBddCacheData * entry, int l, int r, BddNode* res, int applyop) {
  int success = 0;

  //acquire the lock
  while(__sync_val_compare_and_swap(&entry->lock,FALSE,TRUE) == TRUE) 
  {
    //spin while you didn't acquire
  }
  //make sure you have the lock
  assert(entry->lock == TRUE);

  //update the cache
  if (entry->res != res) {
		entry->a = l;
		entry->b = r;
        entry->invalid = FALSE;
		entry->c = applyop;
		entry->res = res;
  }
  
  //release the lock
  assert(__sync_val_compare_and_swap(&entry->lock,TRUE,FALSE) == TRUE);
}
