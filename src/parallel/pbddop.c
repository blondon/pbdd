#include "pbddop.h"
#include <stdlib.h>

int apply_symm = 0;
const BDD IN_PROGRESS = -1;

/* internal prototypes */
static BDD apply_rec(BDD l, BDD r);
static void apply_par(BDD, BDD);

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
		while (res > 0 && successFlag = 0) {
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
		break;
	}
	
	#ifdef CACHESTATS
		bddcachestats.uniqueMiss++;
	#endif
	
	/* Update bdd node counters*/
	if (__sync_sub_and_fetch(&bddfreenum,1) < 0)
	{
		printf("Error: no more free slots")
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

BDD pbdd_check_terminal_case(BDD l, BDD r)
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


static BDD apply_rec(BDD l, BDD r)
{
	BddCacheData *entry;
	BDD res;

	switch (applyop)
	{
		case bddop_and:
			if (l == r)
				return l;
			if (ISZERO(l)  ||  ISZERO(r))
				return 0;
			if (ISONE(l))
				return r;
			if (ISONE(r))
				return l;
			break;
		case bddop_or:
			if (l == r)
				return l;
			if (ISONE(l)  ||  ISONE(r))
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
			if (ISONE(l)  ||  ISONE(r))
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
		res = oprres[applyop][l<<1 | r];
	else
	{
		entry = BddCache_lookup(&applycache, APPLYHASH(l,r,applyop));

		if (entry->a == l  &&  entry->b == r  &&  entry->c == applyop)
		{
			#ifdef CACHESTATS
			bddcachestats.opHit++;
			#endif
			return entry->r.res;
		}
		#ifdef CACHESTATS
		bddcachestats.opMiss++;
		#endif

		int i;
		if (LEVEL(l) == LEVEL(r))
		{
			cilk_spawn apply_par(LOW(l), LOW(r));
			cilk_spawn apply_par(HIGH(l), HIGH(r));
			cilk_sync;
			res = bdd_makenode(LEVEL(l), READREF(2), READREF(1));
		}
		else if (LEVEL(l) < LEVEL(r))
		{
			cilk_spawn apply_par(LOW(l), r);
			cilk_spawn apply_par(HIGH(l), r);
			cilk_sync;
			res = bdd_makenode(LEVEL(l), READREF(2), READREF(1));
		}
		else
		{
			cilk_spawn apply_par(l, LOW(r));
			cilk_spawn apply_par(l, HIGH(r));
			cilk_sync;
			res = bdd_makenode(LEVEL(r), READREF(2), READREF(1));
		}

		POPREF(2);

		entry->a = l;
		entry->b = r;
		entry->c = applyop;
		entry->r.res = res;
	}

	return res;
}


static void apply_par(BDD l, BDD r)
{
  BDD res = apply_rec(l, r);
  PUSHREF(res);
}
