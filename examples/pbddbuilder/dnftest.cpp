
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <string>
#include <vector>

#include "parser.h"
#include "traverser.h"

using namespace std;

#define VAR(X) bdd_ithvar(X)

typedef std::map<int,std::vector<std::string> > IntToStringsMap;

int main(int argc, const char* argv[])
{
	// read input params
	if (argc < 2) {
		cerr << "\nUSAGE: ./dnftest <in_file> \n\n";
		return 1;
	}
	string fname = argv[1];
	cout << "\n------------" << endl;
	cout << "DNF TEST" << endl;
	cout << "------------" << endl;
	cout << "File: " << fname << endl;
	
	// load input file
	ifstream fstr(fname.c_str());
	string formula;
	getline(fstr, formula);
	fstr.close();
	
	// parse formula
	Parser parser(formula);
	DNF dnf = parser.getDNF();
	StringToIntMap varcnt = parser.getVariableRefCounts();
	StringToClausesMap var2clauses = parser.getVariableClauseRefs();

	// create naive ordering
	IntToStringsMap varcntinv;
	for (StringToIntMap::const_iterator it = varcnt.begin(); it != varcnt.end(); it++)
	{
		int cnt = it->second;
		if (varcntinv.count(cnt) == 0)
			varcntinv[cnt] = vector<string>();
		varcntinv[cnt].push_back(it->first);
	}
	StringToIntMap varOrder;
	int idx = 0;
	for (IntToStringsMap::reverse_iterator it = varcntinv.rbegin(); it != varcntinv.rend(); it++)
	{
		vector<string> vars = it->second;
		for (vector<string>::const_iterator it2 = vars.begin(); it2 != vars.end(); it2++)
		{
			varOrder[*it2] = idx++;
		}
	}

	// build BDD
	Traverser traverser;
	traverser.setInitNodes(1000000);
	traverser.setInitCache(1000000);
	
	// serial
	bdd res1 = traverser.buildBDD(dnf, varOrder);
	bdd_printtable(res1);
	bdd_done();
	
	// parallel
	pBDD res2 = traverser.pbuildBDD(dnf, varOrder);
	pbdd_print(res2.node());
	pbdd_done();
	
	cout << endl;
	return 0;
}
