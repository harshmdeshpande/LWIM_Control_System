#ifndef TRAJECTORYCONTROLLER_H_
#define TRAJECTORYCONTROLLER_H_

#include "../curves/Generator.h"
#include "../kinematics/KinematicsSolver.h"
#include "../curves/Spline.h"
#include "../curves/SplineSet.h"

#include <roboop/robot.h>
#include <roboop/trajectory.h>
#include <vector.h>

class TrajectoryController {
public:
	TrajectoryController(KinematicsSolver* kineSolver);
	virtual ~TrajectoryController();

	vector<ColumnVector> getJSPoints(vector<ColumnVector>& cartesianList);
	Spl_path* createSpline(vector<ColumnVector>& jsPoints, double duration);
	SplineSet* createSplines(vector<ColumnVector>& jsPoints);
	bool generateJSPath(vector<ColumnVector>& cartesianList);
	bool savePath(vector<ColumnVector>& pts, int ID);
	ReturnMatrix readFile(int pathID);
	vector<int> getFileList();
	double optimizeSpline(vector<Spline*> sp, ColumnVector& maxVelos);
	double scaleSpline(vector<Spline*>sp, double k);

private:

	double time_scale_factor;
	KinematicsSolver* kineSolver;
	Generator* getGenerator(vector<ColumnVector>& cartesianList);

};

#endif /*TRAJECTORYCONTROLLER_H_*/
