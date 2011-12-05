/*
 * FILE:	pbddbuilder.h
 * DESC:	Global header file for data type definitions.
 */

#ifndef PBDDBUILDER_H
#define PBDDBUILDER_H

#include <string>
#include <memory>
#include <set>
#include <vector>
#include <map>
#include <boolstuff/BoolExprParser.h>
#include <bdd.h>

// CLAUSE
class Clause
{
public:
	std::string expr;
	std::vector<std::string> posVars, negVars;
};
typedef std::shared_ptr<Clause> ClausePtr;

// DATA TYPEDEFS
typedef bdd Variable;
typedef boolstuff::BoolExpr<std::string> BoolExprString;
typedef std::vector<ClausePtr> DNF;
typedef std::set<std::string> StringSet;
typedef std::vector<std::string> StringVector;
typedef std::map<std::string,int> StringToIntMap;
typedef std::map<ClausePtr,int> ClauseToIntMap;
typedef std::map<std::string,std::vector<ClausePtr> > StringToClausesMap;

// ITERATORS
typedef StringSet::const_iterator StringSetIter;
typedef StringVector::const_iterator StringVectorIter;
typedef DNF::const_iterator DNFIter;

#endif
