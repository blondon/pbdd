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
	expr_->getDNFTermRoots(inserter(dnf_, dnf_.end()));
	// traverse tree to count variable references and map variables to clauses
	for (DNFIter it = dnf_.begin(); it != dnf_.end(); it++)
	{
		Clause *term = *it;
		StringSet pos, neg;
		term->getTreeVariables(pos, neg);
		for (StringSetIter it = pos.begin(); it != pos.end(); it++)
		{
			string key = *it;
			if (varcnt_.count(key) == 0) {
				varcnt_[key] = 0;
				var2clauses_[key] = vector<Clause*>();
			}
			varcnt_[key] = varcnt_[key] + 1;
			var2clauses_[key].push_back(term);
		}
		for (StringSetIter it = neg.begin(); it != neg.end(); it++)
		{
			string key = *it;
			if (varcnt_.count(key) == 0) {
				varcnt_[key] = 0;
				var2clauses_[key] = vector<Clause*>();
			}
			varcnt_[key] = varcnt_[key] + 1;
			var2clauses_[key].push_back(term);
		}
	}
	// traverse tree again to count references within sums
	for (DNFIter it = dnf_.begin(); it != dnf_.end(); it++)
	{
		Clause *term = *it;
		clausecnt_[term] = 0;
		StringSet pos, neg;
		term->getTreeVariables(pos, neg);
		for (StringSetIter it = pos.begin(); it != pos.end(); it++)
		{
			string key = *it;
			clausecnt_[term] += varcnt_[key];
		}
		for (StringSetIter it = neg.begin(); it != neg.end(); it++)
		{
			string key = *it;
			clausecnt_[term] += varcnt_[key];
		}
	}
}

void Parser::print() const
{
	cout << "Formula    : " << expr_ << endl;
	for (DNFIter it = dnf_.begin(); it != dnf_.end(); it++)
	{
		Clause *term = *it;
		StringSet pos, neg;
		term->getTreeVariables(pos, neg);
		cout << "Term       : " << term << endl;
		cout << "  Positives: " << pos << endl;
		cout << "  Negatives: " << neg << endl;
	}
}

