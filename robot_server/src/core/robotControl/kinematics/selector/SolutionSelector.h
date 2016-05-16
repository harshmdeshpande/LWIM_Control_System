#ifndef SOLUTIONSELECTOR_H_
#define SOLUTIONSELECTOR_H_

#include <vector>
#include <roboop/robot.h>
#include "../SolutionSet.h"

class SolutionSelector
{
public:
	SolutionSelector(ColumnVector currentQ);
	virtual ~SolutionSelector();
	
	double setBestSolution (SolutionSet* set);
	SolutionSet* getBest(); 
	
private:
	std::vector<double> _q;
	double minWeight;
	SolutionSet *bestSet;
};

#endif /*SOLUTIONSELECTOR_H_*/
