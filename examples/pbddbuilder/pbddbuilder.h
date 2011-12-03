/*
 * FILE:	pbddbuilder.h
 * DESC:	Global header file for data type definitions.
 */

#ifndef PBDDBUILDER_H
#define PBDDBUILDER_H
#endif

#include <string>
#include <memory>
#include <set>
#include <vector>
#include <map>
#include <boolstuff/BoolExprParser.h>
#include <bdd.h>

// DATA TYPES
typedef bdd Variable;
typedef boolstuff::BoolExpr<std::string> BoolExprString;
typedef BoolExprString Clause;
typedef std::vector<Clause*> DNF;
typedef std::set<std::string> StringSet;
typedef std::map<std::string,int> StringToIntMap;
typedef std::map<Clause*,int> ClauseToIntMap;
typedef std::map<int,std::vector<Clause*> > IntToClausesMap;

// ITERATORS
typedef StringSet::const_iterator StringSetIter;
typedef DNF::const_iterator DNFIter;

