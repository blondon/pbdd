
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <ctime>

#include "parser.h"
#include "variableorderer.h"
#include "traverser.h"

#define TBB

using namespace std;

#define VAR(X) bdd_ithvar(X)

typedef std::map<int,std::vector<std::string> > IntToStringsMap;

int main(int argc, const char* argv[])
{
	// timing info
	clock_t t0, t1;
	
	// read input params
	if (argc < 2) {
		cerr << "\nUSAGE: ./dnftest <in_file> <version> \n";
		cerr << "  Version: [1] serial [2] without reducers [3] with reducers \n\n";
		return 1;
	}
	string fname = argv[1];
	int version = 1;
	if (argc >= 3) {
		const char* opt = argv[2];
		int v = atoi(opt);
		if (v > 0 && v < 4)
			version = v;
	}
	int initNodes = 10000;
	if (argc >= 4) {
		const char* opt = argv[3];
		int v = atoi(opt);
		initNodes = v;
	}
	int initCache = 10000;
	if (argc >= 5) {
		const char* opt = argv[4];
		int v = atoi(opt);
		initCache = v;
	}
	cout << "\n------------" << endl;
	cout << "DNF TEST" << endl;
	cout << "------------" << endl;
	cout << "File: " << fname << endl;
	cout << "Version: ";
	switch (version) {
		case 1: cout << "Serial"; break;
		case 2: cout << "Without reducers"; break;
		case 3: cout << "With reducers"; break;
	}
	cout << endl;
	
	// load input file
	ifstream fstr(fname.c_str());
	string formula;
	getline(fstr, formula);
	fstr.close();
	
	// parse formula
	cout << "Parsing ... ";
	t0 = clock();
	Parser parser(formula);
	DNF dnf = parser.getDNF();
	StringToIntMap varcnt = parser.getVariableRefCounts();
	StringToClausesMap var2clauses = parser.getVariableClauseRefs();
	t1 = clock();
	cout << "done." << endl;
	cout << "  Time: " << (t1-t0)/((double)CLOCKS_PER_SEC) << endl;
	
	/// SMART ORDERING
	cout << "Creating order ... ";
	t0 = clock();
	VariableOrderer orderer(parser);
	StringToIntMap varOrder = orderer.getOrdering();
// 	for (StringToIntMap::const_iterator it = varOrder.begin(); it != varOrder.end(); it++)
// 	{
// 		cout << "Var " << it->first << " idx " << it->second << endl;
// 	}
	t1 = clock();
	cout << "done." << endl;
	cout << "  Time: " << (t1-t0)/((double)CLOCKS_PER_SEC) << endl;

	// build BDD
	Traverser traverser;
	traverser.setInitNodes(initNodes);
	traverser.setInitCache(initCache);
	
	// build BDD
	cout << "Starting traversal ... ";
	t0 = clock();
	bdd  res1;
	pBDD res2;
	switch (version) {
		case 1:
			res1 = traverser.buildBDD(dnf, varOrder);
			//bdd_print(res1.id());
			bdd_done();
			break;
		case 2:
			res2 = traverser.buildPBDDSerial(dnf, varOrder);
			//pbdd_print(res2.node());
			pbdd_done();
			break;
		case 3:
			res2 = traverser.buildPBDD(dnf, varOrder);
			//pbdd_print(res2.node());
			pbdd_done();
			break;
	}
	t1 = clock();
	cout << "done." << endl;
	cout << "  Time: " << (t1-t0)/((double)CLOCKS_PER_SEC) << endl;
	
	cout << endl;
	return 0;
}
