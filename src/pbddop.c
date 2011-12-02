#include "pbdd.h"
#include <stdlib.h>
#include <assert.h>
#include <cilk/cilk.h>

const BDD IN_PROGRESS = -1;

/* internal prototypes */
int pbdd_init(int initnodesize, int cachesize)
{
	int r = pbdd_operator_init(cachesize);
	
	if (r < 0)
		pbdd_operator_done();

	return r;
}

void pbdd_done()
{
	pbdd_operator_done();
}

BDD pbdd_makenode(unsigned int level, BDD low, BDD high, int *creat)
{
	unsigned int hash;
	BDD res;
	BDD newb;
	BDD *p;
	int successFlag = 0;
	
	#ifdef CACHESTATS
		bddcachestats.uniqueAccess++;
	#endif
	
	if (low == high)
		return low;
	hash = NODEHASH(level, low, high);
	p = &(HASH(hash));
	
	res = (*p);
	
	while(1) {
		while (res > 0 && successFlag == 0) {
			if ((LEVEL(res) == level) && (LOW(res) == low) && (HIGH(res) == high))
			{
				*creat = 0;
				#ifdef CACHESTATS
					bddcachestats.uniqueHit++;
				#endif
				return res;
			}
			else {
				p = &(NEXT(res));
				res = (*p);
			}
		}
		if ((res = (BDD)__sync_val_compare_and_swap((int *)p, 0, IN_PROGRESS)) != 0) {
			if (res == IN_PROGRESS) {
				while ((*p) == IN_PROGRESS)
				{
				}	
				res = (*p);
			}
		}
		else
			break;
	}
	
	#ifdef CACHESTATS
		bddcachestats.uniqueMiss++;
	#endif
	
	/* Update bdd node counters*/
	if (__sync_sub_and_fetch(&bddfreenum,1) < 0)
	{
		printf("Error: no more free slots");
		exit(-1);
	}
	__sync_add_and_fetch(&bddproduced, 1);
	
	/* Update next free position */
	while (1)
	{
		res = bddfreepos;
		newb = NEXT(res);
		if (res == __sync_val_compare_and_swap(&bddfreepos, res, newb))
			break;
	}
	
	CREATE_NODE(res, level, low, high, 0);
	*p = res;
	*creat = 1;
	return res;
}

static int poprres[MAX_BDD_OP-1][4] = 
{ {0,0,0,1},  /* and                       ( & )         */
  {0,1,1,0},  /* xor                       ( ^ )         */
  {0,1,1,1},  /* or                        ( | )         */
  {1,1,1,0},  /* nand                                    */
  {1,0,0,0},  /* nor                                     */
  {1,1,0,1},  /* implication               ( >> )        */
  {1,0,0,1},  /* bi-implication                          */
  {0,0,1,0},  /* difference /greater than  ( - ) ( > )   */
  {0,1,0,0},  /* less than                 ( < )         */
  {1,0,1,1},  /* inverse implication       ( << )        */
  {1,1,0,0}   /* not                       ( ! )         */
};

BDD pbdd_check_terminal_case(BDD l, BDD r, int applyop)
{
  switch(applyop)
  {
  case bddop_and:
    if (l ==r)
      return l;
    if (ISZERO(l) || ISZERO(r))
      return 0;
    if (ISONE(l))
      return r;
    if (ISONE(r))
      return l;
    break;
  case bddop_or:
    if (l == r)
      return l;
    if (ISONE(l) || ISONE(r))
      return 1;
    if (ISZERO(l))
      return r;
    if (ISZERO(r))
      return l;
    break;
  case bddop_xor:
    if (l == r)
      return 0;
    if (ISZERO(l))
      return r;
    if (ISZERO(r))
      return l;
    break;
  case bddop_nand:
    if (ISZERO(l) || ISZERO(r))
      return 1;
    break;
  case bddop_nor:
    if (ISONE(l) || ISONE(r))
      return 0;
    break;
  case bddop_imp:
    if (ISZERO(l))
      return 1;
    if (ISONE(l))
      return r;
    if (ISONE(r))
      return 1;
    break;
  }
  if (ISCONST(l)  &&  ISCONST(r))
     return poprres[applyop][l<<1 | r];
  return INVALID_BDD;
}


BDD pbdd_apply_rec(BDD l, BDD r, int applyop)
{
	BDD res;
	BDD low, high;
	unsigned int level;
	pBddCacheData *entry;
	hash_t hash;
	int creat;
	
	res = pbdd_check_terminal_case(l,r,applyop);
	if (res != INVALID_BDD)
		return res;
	
	hash = APPLYHASH(l,r);
	cur_cache = pbdd_get_applycache(applyop);
	entry = pBddCache_lookup(cur_cache,hash);
	
	/* acquire lock on cache entry */
	pthread_rwlock_rdlock(&entry->lock);
	
	if (entry->a == l  &&  entry->b == r)
	{
		#ifdef CACHESTATS
		bddcachestats.opHit++;
		#endif
		
		assert(entry->r.res != INVALID_BDD);
		res = entry->r.res;
		pthread_rwlock_unlock(&entry->lock);
		return res;
	}
	pthread_rwlock_unlock(&entry->lock);
	#ifdef CACHESTATS
	bddcachestats.opMiss++;
	#endif	
	if (LEVEL(l) == LEVEL(r))
	{
		level = LEVEL(l);
		low = cilk_spawn  pbdd_apply_rec(LOW(l), LOW(r), applyop);
		high = pbdd_apply_rec(HIGH(l), HIGH(r), applyop);
	}
	else if (LEVEL(l) < LEVEL(r))
	{
		level = LEVEL(l);
		low = cilk_spawn pbdd_apply_rec(LOW(l), r, applyop);
		high = pbdd_apply_rec(HIGH(l), r, applyop);
	}
	else
	{
		level = LEVEL(r);
		low = cilk_spawn pbdd_apply_rec(l, LOW(r), applyop);
		high = pbdd_apply_rec(l, HIGH(r), applyop);
	}
	cilk_sync;
	res = pbdd_makenode(level, low, high, &creat);

	pthread_rwlock_wrlock(&entry->lock);
		
	if (entry->r.res != res) {
		entry->a = l;
		entry->b = r;
		entry->c = applyop;
		entry->r.res = res;
	}
	pthread_rwlock_unlock(&entry->lock);
	return res;
}


BDD pbdd_apply(BDD l, BDD r, int applyop)
{
  BDD res = pbdd_apply_rec(l, r,applyop);
  return res;
}
