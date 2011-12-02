/*
 * FILE:	pbddbuilder.h
 * DESC:	Global header file for data type definitions.
 */

#ifndef PBDDBUILDER_H
#define PBDDBUILDER_H
#endif

#include <string>
#include <memory>
#include <map>
#include <set>
#include <vector>
#include <boolstuff/BoolExprParser.h>

typedef boolstuff::BoolExpr<std::string> BoolExprString;
typedef std::vector< const boolstuff::BoolExpr<std::string> *> DNF;
typedef DNF::iterator DNFIter;
typedef std::set<std::string> StringSet;
typedef StringSet::iterator StringSetIter;
typedef bdd Variable;
typedef BoolExprString Clause;
typedef std::unordered_map<std::string,int> StringToIntMap;
typedef std::unordered_map<Clause*,int> ClauseToIntMap;
typedef std::unordered_map<int,std::vector<Clause*> > IntToClausesMap;

