#include "SolutionSelector.h"

#include <iostream>
#include <math.h>

using namespace std;

const double _POWER[6] = { 15000, 15000, 5000, 5000, 2000 };

SolutionSelector::SolutionSelector(ColumnVector currentQ) {
	this->minWeight = 100000;
	//	this->_q.assign( currentQ, currentQ + 5 ); 
	for (int i = 1; i <= 6; i++) {
		this->_q.push_back(currentQ(i));
	}
	this->bestSet = NULL; //new SolutionSet();
}

SolutionSelector::~SolutionSelector() {
}

double SolutionSelector::setBestSolution(SolutionSet *set) {
	double weight = 0;

	//cout << _q.size() << endl;

	for (unsigned int i = 0; i < 6; i++) {
		//		weight = weight + fabs(set->theta[i]); // - _q[i]) * _POWER[i];
		weight = weight + (fabs(set->theta[i] - _q.at(i)) * _POWER[i]);
	}
	//	cout << "weight " << weight << endl;

	//	cout << "Set [ " 
	//	<< set->theta[0] << ", " 
	//	<< set->theta[1] << ", "
	//	<< set->theta[2] << ", "
	//	<< set->theta[3] << ", "
	//	<< set->theta[4] << ", "		
	//	<< set->theta[5] << " ]  \t\t weight = " << weight << endl;
	//	
	//	cout << "==============================================" << endl <<endl;


	if (weight < this->minWeight || fabs(weight - this->minWeight) < 0.0001) {
		this->minWeight = weight;
		this->bestSet = set;
	}

	return weight;
}

SolutionSet* SolutionSelector::getBest() {
	return bestSet;
}
