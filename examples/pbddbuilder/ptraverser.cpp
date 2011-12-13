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

/// PARALLEL METHODS ///

pBDD Traverser::buildPBDD(const DNF& dnf, const StringToIntMap& varOrder)
{
	// init BuDDy
	if (pbdd_isrunning() == 0) {
		pbdd_init(initNodes_, initCache_);
	}
	// use cilk reducer to execute conjunctions in parallel
	reducer_opor<pBDDS> res;
	cilk_for (DNFIter it = dnf.begin(); it != dnf.end(); it++)
	{
		const ClausePtr clause = *it;
		res = res | buildTermPBDD(clause, varOrder);
	}
	return res.get_value();
}

pBDDS Traverser::buildTermPBDD(const ClausePtr clause, const StringToIntMap& varOrder)
{
	bool noPosTerms = clause->posVars.size() == 0;
	bool noNegTerms = clause->negVars.size() == 0;
 	reducer_opand<pBDDS> res;
	if (!noPosTerms)
	{
		string a = clause->posVars[0];
		int aidx = varOrder.find(a)->second;
		pBDDS avar = pbdd_ithvar(aidx);
		res.set_value(avar);
		cilk_for (int i = 1; i < clause->posVars.size(); i++)
		{
			string b = clause->posVars[i];
			int bidx = varOrder.find(b)->second;
			pBDDS bvar = pbdd_ithvar(bidx);
			res = res & bvar;
		}
	}
// 	if (!noNegTerms)
// 	{
// 		int offset = 0;
// 		if (noPosTerms)
// 		{
// 			string a = clause->negVars[0];
// 			int aidx = varOrder.find(a)->second;
// 			pBDD avar = pbdd_nithvar(aidx);
// 			res.set_value(avar);
// 			offset = 1;
// 		}
// 		cilk_for (int i = offset; i < clause->negVars.size(); i++)
// 		{
// 			string b = clause->negVars[i];
// 			int bidx = varOrder.find(b)->second;
// 			pBDD bvar = bdd_nithvar(bidx);
// 			res &= bvar;
// 		}
// 	}
	return res.get_value();
}

/// SERIAL METHODS ///

pBDD Traverser::buildPBDDSerial(const DNF& dnf, const StringToIntMap& varOrder)
{
	// init BuDDy
	if (pbdd_isrunning() == 0) {
		pbdd_init(initNodes_, initCache_);
	}
	// use cilk reducer to execute conjunctions in parallel
	pBDD res;
	for (DNFIter it = dnf.begin(); it != dnf.end(); it++)
	{
		const ClausePtr clause = *it;
		res = res | buildTermPBDDSerial(clause, varOrder);
	}
	return res;
}

pBDD Traverser::buildTermPBDDSerial(const ClausePtr clause, const StringToIntMap& varOrder)
{
	bool noPosTerms = clause->posVars.size() == 0;
	bool noNegTerms = clause->negVars.size() == 0;
 	pBDD res;
	if (!noPosTerms)
	{
		string a = clause->posVars[0];
		int aidx = varOrder.find(a)->second;
		pBDD avar = pbdd_ithvar(aidx);
		res = avar;
		for (int i = 1; i < clause->posVars.size(); i++)
		{
			string b = clause->posVars[i];
			int bidx = varOrder.find(b)->second;
			pBDD bvar = pbdd_ithvar(bidx);
			res = res & bvar;
		}
	}
	return res;
}


