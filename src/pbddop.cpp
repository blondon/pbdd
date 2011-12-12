#include "pbdd.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <cilk/cilk.h>
#include <sstream>
#include <iostream>

using namespace tbb;
using namespace std;

BddNode * ONE;
BddNode * ZERO;
UniqueTable bddNodes;
bool _isRunning = false;

/* internal prototypes */
int pbdd_init(int initnodesize, int cachesize)
{
   int r = pbdd_operator_init(cachesize);
	
   if (r < 0)
  	pbdd_operator_done();
   //Make zero node
   ZERO = (BddNode*)malloc(sizeof(BddNode));
   ZERO->level = 0;
   ZERO->key = 0;
   ZERO->low = NULL;
   ZERO->high = NULL;
   //Make one nodes
   ONE = (BddNode*)malloc(sizeof(BddNode));
   ONE->level = 1;
   ONE->key = 0;
   ONE->low = NULL;
   ONE->high = NULL;
   _isRunning = true;
 
   return r;
}

bool pbdd_isrunning(){
  return _isRunning;
}

void pbdd_done()
{
   pbdd_operator_done();
}

BddNode* pbdd_makenode(unsigned int level, BddNode* low, BddNode* high){
    BddNode *newNode;
    //create node key
    string key;
    stringstream out;
    out << level << (size_t)low << (size_t)high;
    key = out.str();
    //lookupInsert key in UniqueTable
    UniqueTable::accessor a;
    if (bddNodes.insert(a,key)) {
      //create new node 
      if ((newNode=(BddNode*)malloc(sizeof(BddNode))) == NULL)
        exit(-1);
      newNode->level = level;
      newNode->key = atoi(key.c_str());
      newNode->low = low;
      newNode->high = high;
      a->second = newNode;
    }
    else
      newNode = a->second;
    return newNode;
}

BddNode* pbdd_ithvar(int level) 
{
    BddNode *newNode;
    //create node key
    int internalLevel = level + 2; 
    string key;
    stringstream out;
    out << internalLevel;
    key = out.str();
    //insert node in UniqueTable
    UniqueTable::accessor a;
    if (bddNodes.insert(a,key)) {
      //create new node 
      if ((newNode=(BddNode*)malloc(sizeof(BddNode))) == NULL)
        exit(-1);
      newNode->level = internalLevel;
      newNode->low = ZERO;
      newNode->key = atoi(key.c_str());
      newNode->high = ONE;
      a->second = newNode;
    }
    else
      newNode = a->second;
    return newNode;
}

BddNode* pbdd_check_terminal_case(BddNode* lbdd, BddNode* rbdd, int applyop)
{
  unsigned int l = LEVELp(lbdd);
  unsigned int r = LEVELp(rbdd);
  switch(applyop)
  {
  case bddop_and:
    if (l == r)
      return lbdd;
    if (ISZERO(l) || ISZERO(r))
      return ZERO;
    if (ISONE(l))
      return rbdd;
    if (ISONE(r))
      return lbdd;
    break;
  case bddop_or:
    if (l == r)
      return lbdd;
    if (ISONE(l) || ISONE(r))
      return ONE;
    if (ISZERO(l))
      return rbdd;
    if (ISZERO(r))
      return lbdd;
    break;
  case bddop_xor:
    if (l == r)
      return ZERO;
    if (ISZERO(l))
      return rbdd;
    if (ISZERO(r))
      return lbdd;
    break;
  case bddop_nand:
    if (ISZERO(l) || ISZERO(r))
      return ONE;
    break;
  case bddop_nor:
    if (ISONE(l) || ISONE(r))
      return ZERO;
    break;
  case bddop_imp:
    if (ISZERO(l))
      return ONE;
    if (ISONE(l))
      return rbdd;
    if (ISONE(r))
      return ONE;
    break;
  }
  return NULL;
}

BddNode* pbdd_apply_rec(BddNode* l, BddNode* r, int applyop)
{
	BddNode* res;
	BddNode* low;
        BddNode* high;
	unsigned int level;
	pBddCacheData *entry;
	hash_t hash;
	res = pbdd_check_terminal_case(l,r,applyop);
	if (res != NULL) 
	   return res;
	
	hash = APPLYHASH(l->level,r->level);
	cur_cache = pbdd_get_applycache(applyop);
	entry = pBddCache_lookup(cur_cache, hash);
	res = pBddCache_read(entry,l->key,r->key);
	if (res != NULL) 
	  return res;
    
    
	if (LEVELp(l) == LEVELp(r))
	{
		level = LEVELp(l);
		low = cilk_spawn pbdd_apply_rec(LOWp(l), LOWp(r), applyop);
		high = pbdd_apply_rec(HIGHp(l), HIGHp(r), applyop);
		cilk_sync;
	}
	else if (LEVELp(l) < LEVELp(r))
	{
		level = LEVELp(l);
		low = cilk_spawn  pbdd_apply_rec(LOWp(l), r, applyop);
		high = pbdd_apply_rec(HIGHp(l), r, applyop);
		cilk_sync;
	}
	else
	{
		level = LEVELp(r);
		low = cilk_spawn pbdd_apply_rec(l, LOWp(r), applyop);
		high = pbdd_apply_rec(l, HIGHp(r), applyop);
		cilk_sync;
	}

    res = pbdd_makenode(level, low, high);	
	pBddCache_insert(entry, l->key, r->key, res, applyop);
	return res;
}

BddNode* pbdd_apply_ser(BddNode* l, BddNode* r, int applyop)
{
	BddNode* res;
	BddNode* low;
        BddNode* high;
	unsigned int level;
	pBddCacheData *entry;
	hash_t hash;
	res = pbdd_check_terminal_case(l,r,applyop);
	if (res != NULL) 
	   return res;
	
	hash = APPLYHASH(l->level,r->level);
	cur_cache = pbdd_get_applycache(applyop);
// 	entry = pBddCache_lookup(cur_cache, hash);
// 	res = pBddCache_read(entry,l->key,r->key);
// 	if (res != NULL) 
// 	  return res;
       
	if (LEVELp(l) == LEVELp(r))
	{
		level = LEVELp(l);
		low = pbdd_apply_ser(LOWp(l), LOWp(r), applyop);
		high = pbdd_apply_ser(HIGHp(l), HIGHp(r), applyop);
	}
	else if (LEVELp(l) < LEVELp(r))
	{
		level = LEVELp(l);
		low = pbdd_apply_ser(LOWp(l), r, applyop);
		high = pbdd_apply_ser(HIGHp(l), r, applyop);
	}
	else
	{
		level = LEVELp(r);
		low = pbdd_apply_ser(l, LOWp(r), applyop);
		high = pbdd_apply_ser(l, HIGHp(r), applyop);
	}

    res = pbdd_makenode(level, low, high);	
	//pBddCache_insert(entry, l->key, r->key, res, applyop);
	return res;
}

BddNode* pbdd_apply(BddNode* l, BddNode* r, int applyop)
{
	BddNode* res = pbdd_apply_ser(l, r, applyop);
	return res;
}

void pbdd_printnode(BddNode* node)
{
	if (node->level == 0)
		printf("  -");
	else if (node->level == 1)
		printf("  +");
	else
		printf("%3d", node->level-2);
}

void pbdd_print(BddNode* root)
{
	if (root->level != 0 && root->level != 1) {
		printf("%5d :", root->level-2);
		pbdd_printnode(root->low);
		pbdd_printnode(root->high);
		printf("\n");
		pbdd_print(root->low);
		pbdd_print(root->high);
	} 
}


