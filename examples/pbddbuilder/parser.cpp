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
	_expr = parser.parse(formula);
	_expr = BoolExprString::getDisjunctiveNormalForm(_expr);
	_expr->getDNFTermRoots(inserter(_dnf, _dnf.end()));
	// traverse tree to count variable references and map variables to clauses
	for (DNFIter it = _dnf.begin(); it != _dnf.end(); it++)
	{
		Clause *term = *it;
		StringSet pos, neg;
		term->getTreeVariables(pos, neg);
		for (StringSetIter it = pos.begin(); it != pos.end(); it++)
		{
			string key = *it;
			if (_varcnt.count(key) == 0) {
				_varcnt[key] = 0;
				_var2clauses[key] = vector<Clause*>();
			}
			_varcnt[key] = _varcnt[key] + 1;
			_var2clauses[key].push_back(term);
		}
		for (StringSetIter it = neg.begin(); it != neg.end(); it++)
		{
			string key = *it;
			if (_varcnt.count(key) == 0) {
				_varcnt[key] = 0;
				_var2clauses[key] = vector<Clause*>();
			}
			_varcnt[key] = _varcnt[key] + 1;
			_var2clauses[key].push_back(term);
		}
	}
	// traverse tree again to count references within sums
	for (DNFIter it = _dnf.begin(); it != _dnf.end(); it++)
	{
		Clause *term = *it;
		_clausecnt[term] = 0;
		StringSet pos, neg;
		term->getTreeVariables(pos, neg);
		for (StringSetIter it = pos.begin(); it != pos.end(); it++)
		{
			string key = *it;
			_clausecnt[term] += _varcnt[key];
		}
		for (StringSetIter it = neg.begin(); it != neg.end(); it++)
		{
			string key = *it;
			_clausecnt[term] += _varcnt[key];
		}
	}
}

void Parser::print() const
{
	cout << "Formula    : " << _expr << endl;
	for (DNFIter it = _dnf.begin(); it != _dnf.end(); it++)
	{
		Clause *term = *it;
		StringSet pos, neg;
		term->getTreeVariables(pos, neg);
		cout << "Term       : " << term << endl;
		cout << "  Positives: " << pos << endl;
		cout << "  Negatives: " << neg << endl;
	}
}

