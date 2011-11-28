#include "pbddop.h"


int pbdd_init(int initnodesize, int cachesize)
{
	int r = pbdd_operator_init(cachesize);
	
	if (r < 0)
		pbdd_operator_done();
	return r;
}

void pbdd_done(void)
{
	pdd_operator_done();
}