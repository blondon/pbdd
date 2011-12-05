/*
 * FILE:	traverser.h
 * DESC:	Builds a BDD from a given DNF and ordering.
 */

#ifndef TRAVERSER_H
#define TRAVERSER_H

#include <pbddbuilder.h>

class Traverser
{
public:

	Traverser();
	
	~Traverser();
	
	bdd buildBDD(const DNF& dnf, const StringToIntMap& varOrder);
	
	bdd pbuildBDD(const DNF& dnf, const StringToIntMap& varOrder);

private:
	
	bdd buildTermBDD(const ClausePtr term, const StringToIntMap& varOrder);

	bdd pbuildTermBDD(const ClausePtr term, const StringToIntMap& varOrder);

};

#endif
