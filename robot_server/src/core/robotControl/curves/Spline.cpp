#include "Spline.h"

bool DEBUG = false;

Spline::Spline(Matrix& pts) {
	//	cout << pts << endl;
	this->pts = pts;
	n = pts.Ncols();

	time.ReSize(n);
	time = pts.Row(1).as_column();

	initSplineData();
	findCoeffs();
}

void Spline::initSplineData() {

	A.ReSize(n);
	B.ReSize(n-1);
	D.ReSize(n-1);
	H.ReSize(n-1);
	M.ReSize(n);

	C.ReSize(n);
	L.ReSize(n);
	U.ReSize(n);
	Z.ReSize(n);
}

void Spline::findCoeffs() {
	//	pts.getRow(1, A);
	A = pts.Row(2).AsColumn();
	//	cout << A << endl;

	double h = time(2) - time(1);
	H(1) = h;

	double m = 3 * (A(2) - A(1)) / H(1);
	M(1) = m;

	h = time(n) - time(n - 1);
	H(n - 1) = h;

	m = -3 * (A(n) - A(n - 1)) / H(n - 1);
	M(n) = m;

	double l = 2 * H(1);
	L(1) = l;

	U(1) = 0.5;

	double z = M(1) / L(1);
	Z(1) = z;

	for (int i = 2; i <= n - 1; i++) {
		h = time(i + 1) - time(i);
		H(i) = h;
		//		cout << H << endl;

		m = (3 * (A(i + 1) - A(i)) / H(i)) - (3 * (A(i) - A(i - 1)) / H(i - 1));
		M(i) = m;

		l = 2 * (time(i + 1) - time(i - 1)) - H(i - 1) * U(i - 1);
		L(i) = l;

		double u = H(i) / L(i);
		U(i) = u;

		z = (M(i) - H(i - 1) * Z(i - 1)) / L(i);
		Z(i) = z;
	}

	l = H(n - 1) * (2 - U(n - 1));
	L(n) = l;

	z = (M(n) - H(n - 1) * Z(n - 1)) / L(n);
	Z(n) = z;

	C(n) = z;

	if (DEBUG) {
		cout << "H " << H.as_row() << endl;
		cout << "M " << M.as_row() << endl;
		cout << "L " << L.as_row() << endl;
		cout << "U " << U.as_row() << endl;
		cout << "Z " << Z.as_row() << endl;
	}

	for (int j = n - 1; j >= 1; j--) {
		double c = Z(j) - U(j) * C(j + 1);
		C(j) = c;

		double d = (C(j + 1) - C(j)) / (3 * H(j));
		D(j) = d;

		double b = ((A(j + 1) - A(j)) / H(j)) - (H(j) * (C(j + 1) + 2 * C(j))
				/ 3);
		B(j) = b;
	}

	if (DEBUG) {
		cout << "A " << A.as_row() << endl;
		cout << "B " << B.as_row() << endl;
		cout << "C " << C.as_row() << endl;
		cout << "D " << D.as_row() << endl;
	}
}

double* Spline::interpolate(double t) {
	double* x = new double[4];

	for (int i = 1; i < time.size(); i++) {
		if (t >= time(i) && t <= time(i + 1)) {
			// cout << time(i) << endl;
			double h = t - time(i);
			x[0] = A(i) + B(i) * h + C(i) * h * h + D(i) * h * h * h;

			x[1] = B(i) + 2 * C(i) * h + 3 * D(i) * h * h;

			x[2] = 2 * C(i) + 6 * D(i) * h;

			x[3] = 6 * D(i);

			return x;
		}
		// cout << "time " << t) << endl;
	}
	cout << "ERROR : t is out of range." << endl;
	return NULL;
}

void Spline::scale(double k) {
	B = B / k;
	C = C / (k * k);
	D = D / (k * k * k);

	time = time * k;
}

double Spline::getOptimumTime(double maxEPOS) {
	double max = 0;

	for (int i = 1; i < time.size(); i++) {

		double t = time(i) - C(i) / (3 * D(i));
		if (!(t >= time(i) && t <= time(i + 1))) {
			t = time(i + 1);
		}
		double h = t - time(i);
		double maxVelo = B(i) + 2 * C(i) * h + 3 * D(i) * h * h;

		double ratio = fabs(maxVelo) / maxEPOS;
		//		cout << "ratio " << ratio << endl;

		if (max < ratio) {
			max = ratio;
		}
	}
	return max;
}

Spline::~Spline() {
}
