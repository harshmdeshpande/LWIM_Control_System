#ifndef SOLUTIONSET_H_
#define SOLUTIONSET_H_

#include <iostream>
#include <math.h>

struct SolutionSet {
	double theta[6];
	double weight;

	SolutionSet() {
		SolutionSet(0, 0, 0, 0, 0, 0);
	}

	SolutionSet(double t1, double t2, double t3, double t4, double t5, double t6) {
		theta[0] = t1;
		theta[1] = t2;
		theta[2] = t3;
		theta[3] = t4;
		theta[4] = t5;
		theta[5] = t6;
	}
	
	void show() {
		std::cout << "Solution Set" << std::endl;
		std::cout<< "[ ";
		for (int var = 0; var < 6; var++) {
			std::cout << theta[var] << " ";
		}
		std::cout<<"] : weight " << weight <<std::endl;
		
	}
	
	void showDegrees() {
		std::cout << "Solution Set" << std::endl;
		std::cout<< "[ ";
		for (int var = 0; var < 6; var++) {
			std::cout << theta[var] * 180 / M_PI << " ";
		}
		std::cout<<"]"<<std::endl;
		
	}
};
	
#endif /*SOLUTIONSET_H_*/
