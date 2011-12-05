/*
 * FILE:	variableorderer.h
 * DESC:	Creates a variable ordering for the construction of a BDD,
 * 			given the parse tree of a Boolean formula.
 */

#ifndef VARIABLEORDERER_H
#define VARIABLEORDERER_H

#include <pbddbuilder.h>
#include "parser.h"

typedef std::map<int,DNF> IntToClausesMap;
typedef IntToClausesMap::const_iterator IntToClausesMapIter;
typedef ClauseToIntMap::const_reverse_iterator ClauseRevIter;

class VariableOrderer
{
public:

	VariableOrderer(const Parser& parser);
	
	~VariableOrderer();

	const StringToIntMap& getOrdering() const { return _ordering; }

private:

	class Coloring;
	template <class T>
	class MapComparator;

	Coloring color(const Parser& parser);

	void order(const Parser& parser, VariableOrderer::Coloring coloring);
	
	StringToIntMap _ordering;
	
	/*
	 * A coloring such that each set of clauses connected by at least
	 * one shared variable have the same color.
	 */
	class Coloring
	{
	public:
		/* Deletes the ClauseLists in invClauseColoring. */
		~Coloring();
		
		StringToIntMap varColoring;
		ClauseToIntMap clauseColoring;
		IntToClausesMap invClauseColoring;
	};
	
	/*
	 * Compares objects based on the natural ordering of the integers
	 * to which they map.
	 */
	template <class T>
	class MapComparator
	{
	public:
		MapComparator(const std::map<T,int>& compMap);
		
		~MapComparator();
	
		bool operator() (const T &obj1, const T &obj2);
		
	private:
		std::map<T,int> compMap;
	};
	
};

#endif
