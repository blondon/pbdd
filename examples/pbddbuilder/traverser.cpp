/*
 * FILE:	traverser.cpp
 * DESC:	Builds a BDD from a given DNF and ordering.
 */

#include "traverser.h"

using namespace std;

Traverser::Traverser()
{
}

Traverser::~Traverser()
{
}

bdd Traverser::buildBDD(const DNF& dnf, const StringToIntMap& varOrder)
{
	// init BuDDy
	if (bdd_isrunning() == 0) {
		bdd_init(10000,10000);
		bdd_setvarnum(varOrder.size());
	}
	else {
		bdd_extvarnum(varOrder.size());
	}
	// iterate over terms; evaluate each conjunction separately
	bdd res;
	for (DNFIter it = dnf.begin(); it != dnf.end(); it++)
	{
		const ClausePtr clause = *it;
		res |= buildTermBDD(clause, varOrder);
	}
	return res;
}

bdd Traverser::buildTermBDD(const ClausePtr clause, const StringToIntMap& varOrder)
{
	bool noPosTerms = clause->posVars.size() == 0;
	bool noNegTerms = clause->negVars.size() == 0;
	bdd avar;
	if (!noPosTerms)
	{
		StringVectorIter it = clause->posVars.begin();
		string a = *it;
		int aidx = varOrder.find(a)->second;
		avar = bdd_ithvar(aidx);
		++it;
		while (it != clause->posVars.end())
		{
			string b = *it;
			int bidx = varOrder.find(b)->second;
			bdd bvar = bdd_ithvar(bidx);
			avar = avar & bvar;
			++it;
		}
	}
	if (!noNegTerms)
	{
		StringVectorIter it = clause->negVars.begin();
		if (noPosTerms)
		{
			string a = *it;
			int aidx = varOrder.find(a)->second;
			avar = bdd_nithvar(aidx);
			++it;
		}
		while (it != clause->negVars.end())
		{
			string b = *it;
			int bidx = varOrder.find(b)->second;
			bdd bvar = bdd_nithvar(bidx);
			avar = avar & bvar;
			++it;
		}
	}
	return avar;
}

