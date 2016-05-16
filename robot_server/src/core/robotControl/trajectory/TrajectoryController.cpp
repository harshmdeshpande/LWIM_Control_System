#include "TrajectoryController.h"

#include <dirent.h>

int PNT_SAMPLES = 10;
string FILE_TAG = "traj_";

inline double round(double num) {
	return ((int)(num * 10000))/10000.0;
}

TrajectoryController::TrajectoryController(KinematicsSolver* kineSolver) {
	this->kineSolver = kineSolver;
}

TrajectoryController::~TrajectoryController() {
	delete this->kineSolver;
}

/*
 *  returns a generator depending on num of via points
 */
Generator* TrajectoryController::getGenerator(vector<ColumnVector>& cartesian) {
	Generator* gen;

	int ptNum = cartesian.size();
	//	cout << "num " << ptNum << endl;
	if (ptNum > 1) {
		switch (ptNum) {
		case 2:
			gen = new GLine(cartesian);
			//			cout << "It's a line!" << endl;
			break;
		case 3:
			gen = new GCircle(cartesian);
			//			cout << "It's a circle!" << endl;
			break;
		case 4:
			gen = new GBezier(cartesian);
			//			cout << "It's a bezier!" << endl;
			break;
		default:
			gen = new GSpline(cartesian); //TODO: check!
			//			cout << "It's a spline!" << endl;
			break;
		}
		return gen;
	}
	return NULL;
}

/*
 *  converts cartesian via points to joint space via points
 */
vector<ColumnVector> TrajectoryController::getJSPoints(
		vector<ColumnVector>& cartesianList) {

	vector<ColumnVector> jsPoints;
	ColumnVector cartesian(6), currentQ(6), previousQ(6), joint(6);
	Generator* gen = getGenerator(cartesianList);
	if (gen != NULL) {
		currentQ = 0;
		joint = 0;

		// - convert ith cartesian to thetas
		// - if conditions satisfied, add point in path.
		for (int i = 0; i <= PNT_SAMPLES; i++) {
			double k = ((i * 1.0) / PNT_SAMPLES);

			//			cout << k << endl;
			//get xyz and orientation from generator for time k.
			cartesian = gen->getPosition(k);
			//			cout << "cart c " << cartesian.as_row();
			Quaternion quat = gen->getOrientation(k);
			cartesian.resize_keep(6);
			Matrix R = quat.R();
			cartesian.Rows(4, 6) = irpy(R);

			//			cout << cartesian.AsRow() << endl;
			//			cout << quat << endl;

			//add the joint solution if it exists
			if (kineSolver->getBestSolution(cartesian, currentQ, joint)) {
				//				cout << "cartesian " << cartesian.AsRow() << endl;
				//				cout << "joint " << joint.AsRow() << endl;
				jsPoints.push_back(joint);
				currentQ = joint;
			} else {
				//path has to be rejected
				cout << cartesian.AsRow()
						<< " : Via point out of workspace. Path rejected."
						<< endl;
				jsPoints.clear();
				break;
			}
		}
	}
	return jsPoints;
}

SplineSet* TrajectoryController::createSplines(vector<ColumnVector>& jsViaPoints) {
	vector<Spline*> splineList;
	SplineSet* ss = new SplineSet();

	double delta = 1.0 / (jsViaPoints.size() - 1);

	for (int i = 0; i < 6; i++) { // looping for m
		// joints

		ColumnVector t(jsViaPoints.size());
		ColumnVector x(jsViaPoints.size());
		Matrix points(2, jsViaPoints.size());

		for (unsigned j = 0; j < jsViaPoints.size(); j++) { // looping for n
			// waypoints
			t(j + 1) = j * delta;

			ColumnVector tmp = jsViaPoints.at(j);
			x(j + 1) = tmp(i + 1);

			points(1, j + 1) = t(j + 1);
			points(2, j + 1) = x(j + 1);
		}

		//		cout << i << " " << points << endl;
		ss->addSpline(new Spline(points));
		//		splineList.push_back(new Spline(points));
	}

	return ss;
}

/*
 *  returns a roboop spline given the joint space via points and total duration
 */
Spl_path* TrajectoryController::createSpline(vector<ColumnVector>& jsPoints,
		double duration) {

	int size = jsPoints.size();
	if (size < 5) {
		return NULL;
	}

	//One row for time + 6 for each joint
	//TODO: ?? + 2 for adding the start and stop positions extra to keep the spline smooth
	Matrix pts(7, size);

	for (int i = 0; i < size; i++) {
		double time = 0.999 * (duration * i) / (size - 1);
		ColumnVector p = jsPoints.at(i);

		int index = i + 1;
		pts(1, index) = time;
		pts.SubMatrix(2, 7, index, index) = p;
	}
	cout << pts << endl;

	Spl_path* sp = new Spl_path(pts);
	return sp;
}

/*  
 *  generates path in joint space and saves it to a trajectory file
 */
bool TrajectoryController::generateJSPath(vector<ColumnVector>& cartesianList) {

	// - get joint space via points
	// - save path

	vector<ColumnVector> jsList = this->getJSPoints(cartesianList);
	if (jsList.size() != 0) {
		//save path
		return this->savePath(jsList, 0);
	}
	return false;
}

bool TrajectoryController::savePath(vector<ColumnVector>& pts, int ID) {
	ostringstream oss;
	ofstream stream;
	unsigned size = pts.size();

	if (size < 1) {
		return false;
	}

	oss << FILE_TAG << ID << ".dat";
	const char* name = oss.str().c_str();
	stream.open(name, ios::out);

	for (unsigned int i = 0; i < size; i++) {
		ColumnVector p = pts.at(i);
		for (int j = 1; j <= 6; j++) {
			cout << p(j) << " ";
			stream << round(p(j)) << " ";
		}
		stream << "\n";
		cout << endl << endl;
	}

	stream.close();
	return true;
}

ReturnMatrix TrajectoryController::readFile(int pathID) {
	ostringstream oss;
	ifstream f;

	//	vector<ColumnVector> pts;
	ColumnVector pts(200);
	int index = 0;
	oss << FILE_TAG << pathID << ".dat";
	const char* name = oss.str().c_str();
	cout << "Reading " << name << endl;

	f.open(name, ios::in);
	if (f.is_open()) {
		//char msg[1000];
		double d;
		while (!f.eof()) {
			index++;
			//f.getline(msg, 1000);
			f >> d;
			//double d = atof(msg);
			pts(index) = d;
			//cout << d << endl;
		}
	} else {
		cout << name << " does not exist." << endl;
	}
	pts.resize_keep(index - 1);
	f.close();
	return pts;
}

vector<int> TrajectoryController::getFileList() {
	vector<int> idList;
	DIR *dp;
	struct dirent *dirp;
	string dir = ".";
	if ((dp = opendir(dir.c_str())) == NULL) {
		cout << "Error(" << errno << ") opening " << dir << endl;
	}

	while ((dirp = readdir(dp)) != NULL) {
		string fileName = string(dirp->d_name);

		int size = fileName.size();
		int pos = fileName.find(".");

		if (pos != -1) {
			string ext = fileName.substr(pos, size - pos);

			if (!ext.compare(".dat")) {

				string idStr = fileName.substr(FILE_TAG.size(), pos
						- FILE_TAG.size());

				int id = atoi(idStr.c_str());
				if (id != 0) {
					idList.push_back(id);
				}
			}
		}
	}
	if (idList.size() > 0) {
		sort(idList.begin(), idList.end());
	}
	closedir(dp);
	return idList;
}

