package com.limb.ui.math;

public interface ICurveFit {

	void fit(double rX, double rY, double camX, double camY);

	double[] getResults();

	void showResult();
}
