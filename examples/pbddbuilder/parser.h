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
	
	const DNF& getDNF() const { return _dnf; }
	DNF& getDNF() { return _dnf; }
	
	const StringToIntMap& getVariableRefCounts() const { return _varcnt; }
	StringToIntMap& getVariableRefCounts() { return _varcnt; }
	
	const ClauseToIntMap& getClauseRefCounts() const { return _clausecnt; }
	ClauseToIntMap& getClauseRefCounts() { return _clausecnt; }
	
	const StringToClausesMap& getVariableClauseRefs() const { return _var2clauses; }
	StringToClausesMap& getVariableClauseRefs() { return _var2clauses; }
	
	void print() const;

private:
	
	void parse(const std::string& formula);
	
	BoolExprString *_expr;
	DNF _dnf;
	StringToIntMap _varcnt;
	ClauseToIntMap _clausecnt;
	StringToClausesMap _var2clauses;
};

