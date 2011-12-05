/*
 * FILE:	ptraverser.cpp
 * DESC:	Builds a BDD from a given DNF and ordering, IN PARALLEL.
 */

#include "traverser.h"

#include <cilk/cilk.h>
#include <cilk/reducer_opor.h>
#include <cilk/reducer_opand.h>

using namespace std;
using namespace cilk;

bdd Traverser::pbuildBDD(const DNF& dnf, const StringToIntMap& varOrder)
{
	// init BuDDy
	if (bdd_isrunning() == 0) {
		bdd_init(initNodes_, initCache_);
		bdd_setvarnum(varOrder.size());
	}
	else {
		bdd_extvarnum(varOrder.size());
	}
	// use cilk reducer to execute conjunctions in parallel
	reducer_opor<bdd> res;
	cilk_for (DNFIter it = dnf.begin(); it != dnf.end(); it++)
	{
		const ClausePtr clause = *it;
		res |= pbuildTermBDD(clause, varOrder);
	}
	return res.get_value();
}

bdd Traverser::pbuildTermBDD(const ClausePtr clause, const StringToIntMap& varOrder)
{
	bool noPosTerms = clause->posVars.size() == 0;
	bool noNegTerms = clause->negVars.size() == 0;
 	reducer_opand<bdd> res;
	if (!noPosTerms)
	{
		string a = clause->posVars[0];
		int aidx = varOrder.find(a)->second;
		bdd avar = bdd_ithvar(aidx);
		res.set_value(avar);
		cilk_for (int i = 1; i < clause->posVars.size(); i++)
		{
			string b = clause->posVars[i];
			int bidx = varOrder.find(b)->second;
			bdd bvar = bdd_ithvar(bidx);
			res &= bvar;
		}
	}
	if (!noNegTerms)
	{
		int offset = 0;
		if (noPosTerms)
		{
			string a = clause->negVars[0];
			int aidx = varOrder.find(a)->second;
			bdd avar = bdd_nithvar(aidx);
			res.set_value(avar);
			offset = 1;
		}
		cilk_for (int i = offset; i < clause->negVars.size(); i++)
		{
			string b = clause->negVars[i];
			int bidx = varOrder.find(b)->second;
			bdd bvar = bdd_nithvar(bidx);
			res &= bvar;
		}
	}
	return res.get_value();
}

