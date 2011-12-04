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
	DNFIter it = dnf.begin();
	const Clause *term = *it;
	bdd res = buildTermBDD(term, varOrder);
	++it;
	while (it != dnf.end())
	{
		term = *it;
		res |= buildTermBDD(term, varOrder);
		++it;
	}
	return res;
}

bdd Traverser::buildTermBDD(const Clause *term, const StringToIntMap& varOrder)
{
	StringSet pos, neg;
	term->getTreeVariables(pos, neg);
	bool noPosTerms, noNegTerms;
	noPosTerms = pos.size() == 0;
	noNegTerms = neg.size() == 0;
	//cout << "Term       : " << term << endl;
	bdd avar;
	if (!noPosTerms)
	{
		//cout << "  Positives:";
		StringSetIter it = pos.begin();
		string a = *it;
		int aidx = varOrder.find(a)->second;
		avar = bdd_ithvar(aidx);
		//cout << " " << a;
		++it;
		while (it != pos.end())
		{
			string b = *it;
			int bidx = varOrder.find(b)->second;
			bdd bvar = bdd_ithvar(bidx);
			avar = avar & bvar;
			//cout << " " << b;
			++it;
		}
		//cout << endl;
	}
	if (!noNegTerms)
	{
		//cout << "  Negatives:";
		StringSetIter it = neg.begin();
		if (noPosTerms)
		{
			string a = *it;
			int aidx = varOrder.find(a)->second;
			avar = bdd_nithvar(aidx);
			//cout << " " << a;
			++it;
		}
		while (it != neg.end())
		{
			string b = *it;
			int bidx = varOrder.find(b)->second;
			bdd bvar = bdd_nithvar(bidx);
			avar = avar & bvar;
			//cout << " " << b;
			++it;
		}
		//cout << endl;
	}
	return avar;
}

