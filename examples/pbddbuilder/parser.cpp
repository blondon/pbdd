/*
 * FILE:	parser.cpp
 * DESC:	Parsers a Boolean formula.
 */

#include "parser.h"

using namespace std;
using namespace boolstuff;

/// Parser IMPLEMENTATION ///

Parser::Parser(const string& formula)
{
	parse(formula);
}

Parser::~Parser()
{
}

void Parser::parse(const string& formula)
{
	// parse input formula
	BoolExprParser parser;
	expr_ = parser.parse(formula);
	expr_ = BoolExprString::getDisjunctiveNormalForm(expr_);
	vector<BoolExprString*> dnf;
	expr_->getDNFTermRoots(inserter(dnf, dnf.end()));
	// traverse tree to count variable references and map variables to clauses
	for (vector<BoolExprString*>::const_iterator it = dnf.begin(); it != dnf.end(); it++)
	{
		BoolExprString *term = *it;
		ClausePtr clause(new Clause());
		clause->expr = term->print();
		StringSet pos, neg;
		term->getTreeVariables(pos, neg);
		for (StringSetIter it = pos.begin(); it != pos.end(); it++)
		{
			string key = *it;
			clause->posVars.push_back(key);
			if (varcnt_.count(key) == 0) {
				varcnt_[key] = 0;
				var2clauses_[key] = vector<ClausePtr>();
			}
			varcnt_[key] = varcnt_[key] + 1;
			var2clauses_[key].push_back(clause);
		}
		for (StringSetIter it = neg.begin(); it != neg.end(); it++)
		{
			string key = *it;
			clause->negVars.push_back(key);
			if (varcnt_.count(key) == 0) {
				varcnt_[key] = 0;
				var2clauses_[key] = vector<ClausePtr>();
			}
			varcnt_[key] = varcnt_[key] + 1;
			var2clauses_[key].push_back(clause);
		}
		dnf_.push_back(clause);
	}
	// traverse tree again to count references within sums
	for (DNFIter it = dnf_.begin(); it != dnf_.end(); it++)
	{
		ClausePtr clause = *it;
		clausecnt_[clause->expr] = 0;
		for (StringVectorIter it = clause->posVars.begin(); it != clause->posVars.end(); it++)
		{
			string key = *it;
			clausecnt_[clause->expr] += varcnt_[key];
		}
		for (StringVectorIter it = clause->negVars.begin(); it != clause->negVars.end(); it++)
		{
			string key = *it;
			clausecnt_[clause->expr] += varcnt_[key];
		}
	}
}

void Parser::print() const
{
	cout << "Formula    : " << expr_ << endl;
	for (DNFIter it = dnf_.begin(); it != dnf_.end(); it++)
	{
		ClausePtr clause = *it;
		cout << "Term       : " << clause->expr << endl;
		cout << "  Positives: {";
		for (StringVectorIter it = clause->posVars.begin(); it != clause->posVars.end(); it++)
			cout << " " << *it;
		cout << " }" << endl;
		cout << "  Negatives: {";
		for (StringVectorIter it = clause->negVars.begin(); it != clause->negVars.end(); it++)
			cout << " " << *it;
		cout << " }" << endl;
	}
}

