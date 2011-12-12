/*
 * FILE:	traverser.h
 * DESC:	Builds a BDD from a given DNF and ordering.
 */

#ifndef TRAVERSER_H
#define TRAVERSER_H

#include <pbddbuilder.h>
#include <bdd.h>
#include <pbdd.h>

class Traverser
{
public:

	Traverser();
	
	~Traverser();
	
	int getInitNodes() const { return initNodes_; }
	void setInitNodes(int initNodes) { initNodes_ = initNodes; }
	
	int getInitCache() const { return initCache_; }
	void setInitCache(int initCache) { initCache_ = initCache; }
	
	bdd buildBDD(const DNF& dnf, const StringToIntMap& varOrder);
	
	pBDD pbuildBDD(const DNF& dnf, const StringToIntMap& varOrder);

private:
	
	bdd buildTermBDD(const ClausePtr term, const StringToIntMap& varOrder);

	pBDD pbuildTermBDD(const ClausePtr term, const StringToIntMap& varOrder);
	
	int initNodes_, initCache_;

};

#endif
