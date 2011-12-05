/*
 * FILE:	variableorderer.cpp
 * DESC:	Creates a variable ordering for the construction of a BDD,
 * 			given the parse tree of a Boolean formula.
 */

#include <stack>
#include "variableorderer.h"

using namespace std;
using namespace boolstuff;

VariableOrderer::VariableOrderer(const Parser& parser)
{
	VariableOrderer::Coloring coloring = color(parser);
	order(parser, coloring);
}

VariableOrderer::~VariableOrderer()
{
	/* Intentionally empty */
}

VariableOrderer::Coloring VariableOrderer::color(const Parser& parser)
{
	VariableOrderer::Coloring coloring;
	StringVector pos, neg;
	string var;
	int nextColor = 0;
	DNF* clauseList;
	
	/* Gets data structures from parser */
	const DNF dnf = parser.getDNF();
	const StringToClausesMap varClauseRefs = parser.getVariableClauseRefs();
	const ClauseToIntMap clauseRefCounts = parser.getClauseRefCounts();
	
	/* Initializes comparator */
	VariableOrderer::MapComparator<ClausePtr> clauseComp(clauseRefCounts);
	
	/* Data structure for traversing connected components */
	stack<string> varsToProcess;
	set<string> varsVisited;
	
	/* Iterates over clauses to assign them colors */
	for (DNFIter cItr = dnf.begin(); cItr < dnf.end(); cItr++)
	{
		/*
		 * If a clause is found that does not already have a color, assigns
		 * it the next one. Then assigns all variables, connected clauses,
		 * and their variables the same color.
		 */
		if (coloring.clauseColoring.count(*cItr) == 0)
		{
			/* Initializes a new clause list */
			clauseList = new DNF();
		
			/* Assigns the clause the next color */
			coloring.clauseColoring.insert(pair<ClausePtr,int>(*cItr, nextColor));
			clauseList->push_back(*cItr);
			/* Collects the clause's variables */
			pos = (*cItr)->posVars;
			neg = (*cItr)->negVars;
			
			/* Adds the positive variables to the stack to process... */
			for (StringVectorIter vItr = pos.begin(); vItr != pos.end(); vItr++)
			{
				varsToProcess.push(*vItr);
			}
			
			/* ...and then adds the negative ones */
			for (StringVectorIter vItr = neg.begin(); vItr != neg.end(); vItr++)
			{
				varsToProcess.push(*vItr);
			}
			
			/* For each var to process, assigns it the next color, then adds al */
			while (!varsToProcess.empty())
			{
				var = varsToProcess.top();
				varsToProcess.pop();
				/* If the variable doesn't already have a color... */
				if (coloring.varColoring.count(var) == 0)
				{
					/* ...assigns it the clause's color */
					coloring.varColoring.insert(pair<string,int>(var, nextColor));
					
					/*
					 * Finds the connected clauses, assigns them the same color,
					 * and collects their variables
					 */
					DNF connected = varClauseRefs.find(var)->second;
					for (DNFIter connectedItr = connected.begin(); connectedItr != connected.end(); connectedItr++)
					{
						if (coloring.clauseColoring.count(*connectedItr) == 0)
						{
							/* Assigns the clause the next color */
							coloring.clauseColoring.insert(pair<ClausePtr,int>(*connectedItr, nextColor));
							clauseList->push_back(*connectedItr);
							/* Collects the clause's variables */
							pos = (*connectedItr)->posVars;
							neg = (*connectedItr)->negVars;
							
							/* Adds the positive variables to the stack to process... */
							for (StringVectorIter vItr = pos.begin(); vItr != pos.end(); vItr++)
							{
								varsToProcess.push(*vItr);
							}
			
							/* ...and then adds the negative ones */
							for (StringVectorIter vItr = neg.begin(); vItr != neg.end(); vItr++)
							{
								varsToProcess.push(*vItr);
							}
						}
					}
				}
			}
			
			/*
			 * Sorts clauseList in ASCENDING order according to total reference counts
			 * and adds it to invClauseColoring
			 */
			sort(clauseList->begin(), clauseList->end(), clauseComp);
			coloring.invClauseColoring.insert(pair<int,DNF*>(nextColor++,clauseList));
		}
		
		return coloring;
	}
}

void VariableOrderer::order(const Parser& parser, VariableOrderer::Coloring coloring)
{
	int nextPosition = 0;
	
	/* Gets data structures from parser */
	const DNF dnf = parser.getDNF();
	const StringToIntMap varRefCounts = parser.getVariableRefCounts();
	const ClauseToIntMap clauseRefCounts = parser.getClauseRefCounts();
	
	/* Initializes comparator */
	VariableOrderer::MapComparator<string> varComp(varRefCounts);
	
	/* Data structures for traversing clauses */
	set<ClausePtr> clausesVisited;
	StringVector vars;
	DNF* clauseList;
	
	/* Iterates over the clauses in DESCENDING order of total reference counts */
	for (ClauseRevIter cItr = clauseRefCounts.rbegin(); cItr != clauseRefCounts.rend(); cItr++)
	{
		/* If a clause hasn't been visited, orders its variables and those of all clauses of the same color */
		if (clausesVisited.count(cItr->first) == 0)
		{
			/* Marks the clause as visited */
			clausesVisited.insert(cItr->first);
			
			/* Collects the clause's variables and sorts them in ASCENDING order of reference counts */
			vars.clear();
			vars.insert(vars.end(), cItr->first->posVars.begin(), cItr->first->posVars.end());
			vars.insert(vars.end(), cItr->first->negVars.begin(), cItr->first->negVars.end());
			sort(vars.begin(), vars.end(), varComp);
			
			/* For each variable, if it isn't in the ordering, adds it */
			for (StringVectorRevIter vItr = vars.rbegin(); vItr != vars.rend(); vItr++)
			{
				if (_ordering.count(*vItr) == 0)
				{
					_ordering.insert(pair<string,int>(*vItr,nextPosition++));
				}
			}
			
			/*
			 * For each clause of the same color, processes its variables. Note that
			 * invClauseColoring should already be sorted in ASCENDING order according
			 * to total reference counts.
			 */
			clauseList = coloring.invClauseColoring[coloring.clauseColoring[cItr->first]];
			for (DNFRevIter colorItr = clauseList->rbegin(); colorItr != clauseList->rend(); colorItr++)
			{
				/* If a clause hasn't been visited, orders its variables */
				if (clausesVisited.count(cItr->first) == 0)
				{
					/* Marks the clause as visited */
					clausesVisited.insert(cItr->first);
			
					/* Collects the clause's variables and sorts them in ASCENDING order of reference counts */
					vars.clear();
					vars.insert(vars.end(), cItr->first->posVars.begin(), cItr->first->posVars.end());
					vars.insert(vars.end(), cItr->first->negVars.begin(), cItr->first->negVars.end());
					sort(vars.begin(), vars.end(), varComp);
			
					/* For each variable, if it isn't in the ordering, adds it */
					for (StringVectorRevIter vItr = vars.rbegin(); vItr != vars.rend(); vItr++)
					{
						if (_ordering.count(*vItr) == 0)
						{
							_ordering.insert(pair<string,int>(*vItr,nextPosition++));
						}
					}
				}
			}
		}
	}
}

VariableOrderer::Coloring::~Coloring()
{
	for (IntToClausesMapIter itr = invClauseColoring.begin(); itr != invClauseColoring.end(); itr++)
	{
		delete itr->second;
	}
}

template <class T>
VariableOrderer::MapComparator<T>::MapComparator(const std::map<T,int>& compMap)
{
	this->compMap = compMap;
}

template <class T>
VariableOrderer::MapComparator<T>::~MapComparator()
{
	/* Intentionally blank */
}

template <class T>
bool VariableOrderer::MapComparator<T>::operator() (const T &obj1, const T &obj2)
{
	return compMap[obj1] < compMap[obj2];
}