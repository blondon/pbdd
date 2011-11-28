#ifndef _PBDDOP_H
#define _PBDDOP_H

#include <assert.h>
#include "/fs/buzz/cs714-fa11-022/localInstalls/include/cilk/cilk.h"
#include <pthread.h>

#include "../kernel.h"
#include "pbdd.h"
#include "pbddcache.h"

#ifdef CPLUSPLUS
extern "C" {
#endif

extern int apply_symm;
extern BDD pbdd_apply_check_const(BDD l, BDD r);
extern BDD parallel_apply(BDD l, BDD r);
extern BDD serial_apply(BDD l, BDD r);

extern BDD pbdd_makenode(unsigned int level, BDD low, BDD high, int *creat);

#ifdef CPLUSPLUS
}
#endif

#endif 