/*
 * FILE:	parser.cpp
 * DESC:	Parsers a Boolean formula.
 */

#include "parser.h"

using namespace std;
using namespace boolstuff;

/// Parser IMPLEMENTATION ///

Parser::Parser(const string formula)
{
	Parse(formula);
}

Parser::~Parser()
{
}

void Parser::Parse(const string formula)
{
	// parse input formula
	BoolExprParser parser;
	_expr = parser.parse(formula);
	_expr = BoolExprString::getDisjunctiveNormalForm(_expr);
	_expr->getDNFTermRoots(inserter(_dnf, _dnf.end()));
	// traverse tree to count variable references
	for (DNFIter it = _dnf.begin(); it != _dnf.end(); it++)
	{
		const Clause *term = *it;
		set<string> pos, neg;
		term->getTreeVariables(pos, neg);
		for (StringSetIter it = pos.begin(); it != pos.end(); it++)
		{
			string key = *it;
			if (_varcnt.count(key) == 0) {
				_varcnt[key] = 0;
			}
			_varcnt[key] = _varcnt[key] + 1;
		}
		for (StringSetIter it = neg.begin(); it != neg.end(); it++)
		{
			string key = *it;
			if (_varcnt.count(key) == 0) {
				_varcnt[key] = 0;
			}
			_varcnt[key] = _varcnt[key] + 1;
		}
	}
	// traverse tree again to count references within sums
	for (DNFIter it = _dnf.begin(); it != _dnf.end(); it++)
	{
		const Clause *term = *it;
		_clausecnt[term] = 0;
		set<string> pos, neg;
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

void Parser::Print() const
{
	cout << "Formula    : " << _expr << endl;
	for (DNF::const_iterator it = _dnf.begin(); it != _dnf.end(); it++)
	{
		const BoolExprString *term = *it;
		set<string> pos, neg;
		term->getTreeVariables(pos, neg);
		cout << "Term       : " << term << endl;
		cout << "  Positives: " << pos << endl;
		cout << "  Negatives: " << neg << endl;
	}
}

