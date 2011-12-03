/*
 * FILE:	parser.h
 * DESC:	Parses a Boolean formula.
 */

#ifndef PARSER_H
#define PARSER_H
#endif

#include <pbddbuilder.h>

class Parser
{
public:

	Parser(const std::string& formula);
	
	~Parser();
	
	int getNumVariables() const { return _varcnt.size(); }
	
	int getNumClauses() const { return _clausecnt.size(); }
	
	const StringToIntMap& getVariableRefCounts() const { return _varcnt; }
	StringToIntMap& getVariableRefCounts() { return _varcnt; }
	
	const ClauseToIntMap& getClauseRefCounts() const { return _clausecnt; }
	ClauseToIntMap& getClauseRefCounts() { return _clausecnt; }
	
	const DNF& getDNF() const { return _dnf; }
	DNF& getDNF() { return _dnf; }
	
	void print() const;

private:
	
	void parse(const std::string& formula);
	
	StringToIntMap _varcnt;
	ClauseToIntMap _clausecnt;
	BoolExprString *_expr;
	DNF _dnf;
};

