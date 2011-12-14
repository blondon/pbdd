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
BddNode* unique_hashmap_bddnodes;
int bddnodesize;
int bddfreenum;
int bddproduced;
bool _isRunning = false;

int unique_hashmap_init(int initnodesize){
	if ((unique_hashmap_bddnodes = (BddNode*)malloc(sizeof(BddNode)*initnodesize)) == NULL)
		return -1;
	bddnodesize = initnodesize;
	bddfreenum = initnodesize;
    bddproduced = 0;
	for(int n=0; n<bddnodesize ; n++)
	{
		unique_hashmap_bddnodes[n].level = 0;
		unique_hashmap_bddnodes[n].low = NULL;
		unique_hashmap_bddnodes[n].high = NULL;
		unique_hashmap_bddnodes[n].key = INVALID_BDD;
	}
	return 0;
}
void unique_hashmap_done() {
	free(unique_hashmap_bddnodes);
}

/* internal prototypes */
int pbdd_init(int initnodesize, int cachesize)
{
   //Initialize unique_hashmap_bddNodes
   int r = unique_hashmap_init(initnodesize);
   if (r < 0) {
   	unique_hashmap_done();
   	exit(-1);
   }
   r = pbdd_operator_init(cachesize);
   
   //Init our own local table
	
   if (r < 0) {
  	pbdd_operator_done();
  	exit(-1);
   }
   //Make zero node
   ZERO = (BddNode*)malloc(sizeof(BddNode));
   ZERO->level = 0;
   ZERO->low = NULL;
   ZERO->high = NULL;
   ZERO->key = VALID_BDD;
   //Make one nodes
   ONE = (BddNode*)malloc(sizeof(BddNode));
   ONE->level = 1;
   ONE->low = NULL;
   ONE->high = NULL;
   ONE->key = VALID_BDD;
   _isRunning = true;
   return r;
}

bool pbdd_isrunning(){
  return _isRunning;
}

void pbdd_done()
{
   pbdd_operator_done();
   unique_hashmap_done();
}

BddNode* unique_hashmap_insert(unsigned int level, BddNode* low, BddNode* high){
   BddNode* ent = unique_hashmap_getentry(level,low,high);
   if (ent == NULL)
  	exit(-1);
  if (ent->key == INVALID_BDD) {
    if (__sync_val_compare_and_swap(&ent->key,INVALID_BDD,VALID_BDD) != INVALID_BDD)
      unique_hashmap_insert(level,low,high);
    //we have acquired this entry
    ent->level = level;
    ent->low = low;
    ent->high = high;
  }
  else {
    assert((ent->level == level) && (ent->low == low) && (ent->high == high));
  }
  return ent;
   
}
BddNode* unique_hashmap_lookup(unsigned int level, BddNode* low, BddNode* high) {
  BddNode* ent = unique_hashmap_getentry(level,low,high);
  if (ent != NULL && ent->key != INVALID_BDD)
  	return ent;
  else
    return INVALID_BDD;
}

BddNode* unique_hashmap_getentry(unsigned int level, BddNode* low, BddNode* high) {
   unsigned int hash;
   BddNode* ent;
   int probes;
   
   hash = NODEHASH(level, (size_t)low, (size_t)high);
   ent = &unique_hashmap_bddnodes[hash];
   probes = 0;
   while ((probes < bddnodesize - 1) && (ent->level != level) && (ent->low != low) && (ent->high != high) && (ent->key != INVALID_BDD)){
       probes++;
       hash = (hash + 1) % bddnodesize;
       ent = &unique_hashmap_bddnodes[hash];
   }
   
   if (probes >= bddnodesize - 1)
     return NULL;
   else
     return ent;  
}

BddNode* unique_hashmap_makenode(unsigned int level, BddNode* low, BddNode* high) {
	BddNode* res;
	//look_up node
	res = unique_hashmap_lookup(level,low,high);
	if (res != NULL) {
	  return res;
	}
	//otherwise create
	res = unique_hashmap_insert(level,low,high);
        if (__sync_sub_and_fetch(&bddfreenum,1) < 0)
        {
          exit(-1);
        }
        __sync_add_and_fetch(&bddproduced,1);
	return res;
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
      //newNode->key = key;
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
    #ifdef TBB 
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
        newNode->high = ONE;
        a->second = newNode;
      }
      else
        newNode = a->second;
    #else
      newNode = unique_hashmap_makenode(internalLevel,ZERO,ONE);
      if (newNode == NULL)
    	  exit(-1);
    #endif
    return newNode;
}

BddNode* pbdd_check_terminal_case(BddNode* lbdd, BddNode* rbdd, int applyop)
{
  unsigned int l = LEVELp(lbdd);
  unsigned int r = LEVELp(rbdd);
  switch(applyop)
  {
  case bddop_and:
    if (lbdd == rbdd)
      return lbdd;
    if (ISZERO(l) || ISZERO(r))
      return ZERO;
    if (ISONE(l))
      return rbdd;
    if (ISONE(r))
      return lbdd;
    break;
  case bddop_or:
    if (lbdd == rbdd)
      return lbdd;
    if (ISONE(l) || ISONE(r))
      return ONE;
    if (ISZERO(l))
      return rbdd;
    if (ISZERO(r))
      return lbdd;
    break;
  case bddop_xor:
    if (lbdd == rbdd)
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

BddNode* pbdd_apply(BddNode* l, BddNode* r, int applyop)
{
	BddNode* res = pbdd_apply_rec(l, r, applyop);
	return res;
}

BddNode* pbdd_apply_serial(BddNode* l, BddNode* r, int applyop)
{
	BddNode* res = pbdd_apply_serial_rec(l, r, applyop);
	return res;
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
	res = pBddCache_read(entry, (size_t)l, (size_t)r);
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
    #ifdef TBB
      res = pbdd_makenode(level, low, high);	
    #else
      res =  unique_hashmap_makenode(level, low, high);
    #endif	
       pBddCache_insert(entry, (size_t)l, (size_t)r, res, applyop);
	return res;
}

BddNode* pbdd_apply_serial_rec(BddNode* l, BddNode* r, int applyop)
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
	res = pBddCache_read(entry, (size_t)l, (size_t)r);
	if (res != NULL) 
	  return res;
       
	if (LEVELp(l) == LEVELp(r))
	{
		level = LEVELp(l);
		low = pbdd_apply_serial_rec(LOWp(l), LOWp(r), applyop);
		high = pbdd_apply_serial_rec(HIGHp(l), HIGHp(r), applyop);
	}
	else if (LEVELp(l) < LEVELp(r))
	{
		level = LEVELp(l);
		low = pbdd_apply_serial_rec(LOWp(l), r, applyop);
		high = pbdd_apply_serial_rec(HIGHp(l), r, applyop);
	}
	else
	{
		level = LEVELp(r);
		low = pbdd_apply_serial_rec(l, LOWp(r), applyop);
		high = pbdd_apply_serial_rec(l, HIGHp(r), applyop);
	}
    #ifdef TBB
     res = pbdd_makenode(level, low, high);
    #else
     res =  unique_hashmap_makenode(level, low, high);
    #endif
    	pBddCache_insert(entry, (size_t)l, (size_t)r, res, applyop);
	return res;
}

void pbdd_printnode(const BddNode* node)
{
	if (node->level == 0)
		printf("  -");
	else if (node->level == 1)
		printf("  +");
	else
		printf("%3d", node->level-2);
}

void pbdd_print(const BddNode* root)
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


