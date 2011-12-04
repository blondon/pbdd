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
	
	int getNumVariables() const { return varcnt_.size(); }
	
	int getNumClauses() const { return clausecnt_.size(); }
	
	const DNF& getDNF() const { return dnf_; }
	DNF& getDNF() { return dnf_; }
	
	const StringToIntMap& getVariableRefCounts() const { return varcnt_; }
	StringToIntMap& getVariableRefCounts() { return varcnt_; }
	
	const ClauseToIntMap& getClauseRefCounts() const { return clausecnt_; }
	ClauseToIntMap& getClauseRefCounts() { return clausecnt_; }
	
	const StringToClausesMap& getVariableClauseRefs() const { return var2clauses_; }
	StringToClausesMap& getVariableClauseRefs() { return var2clauses_; }
	
	void print() const;

private:
	
	void parse(const std::string& formula);
	
	BoolExprString *expr_;
	DNF dnf_;
	StringToIntMap varcnt_;
	ClauseToIntMap clausecnt_;
	StringToClausesMap var2clauses_;
};

