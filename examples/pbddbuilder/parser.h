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

	Parser(const std::string formula);
	
	~Parser();
	
	int GetNumVariables() const { return _varcnt.size(); }
	
	int GetNumClauses() const { return _clausecnt.size(); }
	
	const StringToIntMap* GetVariableRefCounts() const { return &_varcnt; }
	StringToIntMap* GetVariableRefCounts() { return &_varcnt; }
	
	const ClauseToIntMap* GetClauseRefCounts() const { return &_clausecnt; }
	ClauseToIntMap* GetClauseRefCounts() { return &_clausecnt; }
	
	void Print() const;

private:
	
	void Parse(const std::string formula);
	
	StringToIntMap _varcnt;
	ClauseToIntMap _clausecnt;
	BoolExprString *_expr;
	DNF _dnf;
};

