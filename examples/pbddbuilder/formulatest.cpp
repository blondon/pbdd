
#include "parser.h"
#include "variableorderer.h"
#include "traverser.h"
#include <list>

using namespace std;

#define VAR(X) bdd_ithvar(X)

typedef std::map<int,std::vector<std::string> > IntToStringsMap;

int main()
{
	/// PARSE FORMULA
	string formula = "(A & B) | (C & (D | E)) | (E & A)";
	Parser parser(formula);
	parser.print();
	DNF dnf = parser.getDNF();
	StringToIntMap varcnt = parser.getVariableRefCounts();
	ClauseToIntMap clausecnt = parser.getClauseRefCounts();
	StringToClausesMap var2clauses = parser.getVariableClauseRefs();
	for (StringToIntMap::const_iterator it = varcnt.begin(); it != varcnt.end(); it++)
	{
		string key = it->first;
		cout << "Var " << key << " cnt " << it->second << endl;
		cout << "  appears in:";
		vector<ClausePtr> clauses = var2clauses.find(key)->second;
		for (vector<ClausePtr>::const_iterator it2 = clauses.begin(); it2 != clauses.end(); it2++)
		{
			cout << " " << (*it2)->expr;
		}
		cout << endl;
	}
	for (ClauseToIntMap::const_iterator it = clausecnt.begin(); it != clausecnt.end(); it++)
	{
		cout << "Clause " << it->first->expr << " cnt " << it->second << endl;
	}
	
	/// SMART ORDERING
	VariableOrderer orderer(parser);
	StringToIntMap varOrder = orderer.getOrdering();
	for (StringToIntMap::const_iterator it = varOrder.begin(); it != varOrder.end(); it++)
	{
		cout << "Var " << it->first << " idx " << it->second << endl;
	}
	
	/// BUILD REFERENCE BDD
	bdd_init(10000,10000);
	bdd_setvarnum(5);
	int a,b,c,d,e;
	a = varOrder["A"];
	b = varOrder["B"];
	c = varOrder["C"];
	d = varOrder["D"];
	e = varOrder["E"];
	bdd res1 = (VAR(a) & VAR(b)) | (VAR(e) & VAR(a)) | (VAR(c) & VAR(d)) | (VAR(c) & VAR(e));
	bdd_print(res1.id());
	bdd_done();
	cout << "Done first test." << endl;
	
	/// BUILD BDD (SERIAL)
	Traverser traverser;
	bdd res2 = traverser.buildBDD(dnf, varOrder);
	bdd_print(res2.id());
	bdd_done();
	cout << "Done second test." << endl;
	
	/// BUILD PBDD (REDUCERS)
	pBDD res3 = traverser.buildPBDD(dnf, varOrder);
	pbdd_print(res3.node());
	pbdd_done();
	cout << "Done third test." << endl;
	
	/// BUILD PBDD (SERIAL)
// 	pBDD res4 = traverser.buildPBDDSerial(dnf, varOrder);
// 	pbdd_print(res4.node());
// 	pbdd_done();
// 	cout << "Done fourth test." << endl;
	
	return 0;
}
