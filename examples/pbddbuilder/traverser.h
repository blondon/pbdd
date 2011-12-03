/*
 * FILE:	traverser.h
 * DESC:	Builds a BDD from a Boolean formula.
 */

#ifndef TRAVERSER_H
#define TRAVERSER_H
#endif

#include <pbddbuilder.h>

class Traverser
{
public:

	Traverser();
	
	~Traverser();
	
	bdd buildBDD(const DNF& dnf, const StringToIntMap& varOrder);

private:
	
	bdd buildTermBDD(const Clause *term, const StringToIntMap& varOrder);

};
