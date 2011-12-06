#ifndef _PBDD_H
#define _PBDD_H

#include <tbb/tbb/concurrent_hash_map.h>
#include <string>


#define bddop_and       0
#define bddop_xor       1
#define bddop_or        2
#define bddop_nand      3
#define bddop_nor       4
#define bddop_imp       5
#define bddop_biimp     6
#define bddop_diff      7
#define bddop_less      8
#define bddop_invimp    9

   /* Should *not* be used in bdd_apply calls !!! */
#define bddop_not      10
#define bddop_simplify 11

#define MAX_BDD_OP (bddop_simplify + 1)
#define TRUE  '1'
#define FALSE '0'


typedef long long hash_t;

typedef struct BddNode BddNode;

struct BddNode {
   unsigned int level;
   int key;
   BddNode *low;
   BddNode *high;
};

//Define Unique table
typedef tbb::concurrent_hash_map<std::string,BddNode*> UniqueTable;



//Define Constants



//Define Cache structure and operations

typedef struct
{
  BddNode* res;
  unsigned char invalid;
  uintptr_t a, b;
  int c;
  volatile unsigned char lock;
} pBddCacheData;

typedef struct
{
  pBddCacheData *table;
  int tablesize;
} pBddCache;


#ifndef APPLYHASH
extern long long APPLYHASH( int l,  int r);
#endif

#define PAIR(a,b)      ((unsigned int)((((unsigned int)a)+((unsigned int)b))*(((unsigned int)a)+((unsigned int)b)+((unsigned int)1))/((unsigned int)2)+((unsigned int)a)))
#define TRIPLE(a,b,c)  ((unsigned int)(PAIR((unsigned int)c,PAIR(a,b))))
#define NODEHASH(lvl,l,h)  (TRIPLE(lvl,l,h) % bddnodesize)
#define NEW(t,n) ( (t*)malloc(sizeof(t)*(n)) )
#define ISCONST(a) ((a) < 2)
#define ISNONCONST(a) ((a) >= 2)
#define ISONE(a)   ((a) == 1)
#define ISZERO(a)  ((a) == 0)
#define LEVELp(p)   ((p)->level)
#define LOWp(p)     ((p)->low)
#define HIGHp(p)    ((p)->high)


extern pBddCache *cur_cache;
extern pBddCache papplycache[MAX_BDD_OP];
extern BddNode * ONE;
extern BddNode * ZERO;
extern UniqueTable bddNodes;



//pbddCache.cpp functions
#define pBddCache_lookup(cache, hash) (&(cache)->table[hash % (cache)->tablesize])


extern int       pBddCache_init(pBddCache *, int);
extern int       pBddCache_done(pBddCache *);
extern int       pBddCache_resize(pBddCache *, int);
extern void      pBddCache_reset(pBddCache *);
extern BddNode*  pBddCache_read(pBddCacheData *, int, int);
extern void      pBddCache_insert(pBddCacheData *,  int,  int,  BddNode*, int);

extern pBddCache *cur_cache;
extern pBddCache papplycache[MAX_BDD_OP];

static inline pBddCache *pbdd_get_applycache(int op)
{
  return &papplycache[op];
}

extern int pbdd_operator_init(int cacheSize);
extern void pbdd_operator_done(void);

//pbddop.cpp functions

extern int      pbdd_init(int initnodesize, int cachesize);
extern void     pbdd_done();
extern BddNode* pbdd_ithvar(int);
extern BddNode* pbdd_check_terminal_case(BddNode* l, BddNode* r, int applyop);
extern BddNode* pbdd_apply_rec(BddNode* l, BddNode* r, int applyop);
extern BddNode* pbdd_apply(BddNode* l, BddNode* r, int applyop);
extern BddNode* pbdd_makenode(unsigned int level, BddNode* low, BddNode* high);
extern void     pbdd_print(BddNode* root);


#endif
