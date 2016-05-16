package com.limb.ui.math;

import java.util.ArrayList;
import java.util.List;

import com.limb.ui.PanelDemo;

import Jama.Matrix;

public class DualRegression implements ICurveFit {
	public List<ObvPoints> obvPointList = new ArrayList<ObvPoints>();
	double mX, nX, jX, mY, nY, jY;
	double last_aX, last_bX, last_cX, last_aY, last_bY, last_cY;

	int count = 1;

	private void calcFit(double robotX, double robotY, double camX, double camY) {

		obvPointList.add(new ObvPoints(robotX, robotY, camX, camY));

		double[][] camArray = createCamArray(obvPointList);
		double[][] rXArray = createRobotXArray(obvPointList);
		double[][] rYArray = createRobotYArray(obvPointList);

		double[][] resultX, resultY;

		Matrix camMat, rXMat, rYMat; // Input Matrices
		Matrix camMatTrans, camProd, rXMatProd, rYMatProd; // Calculation
		// Matrices
		Matrix xFit, yFit;

		if (count >= 3) {

			last_aX = mX;
			last_bX = nX;
			last_cX = jX;

			last_aY = mY;
			last_bY = nY;
			last_cY = jY;

			// getDeviation(robotX, robotY, camX, camY);

			// Multiple Regression
			camMat = new Matrix(camArray);
			rXMat = new Matrix(rXArray);
			rYMat = new Matrix(rYArray);

			camMatTrans = camMat.transpose(); // Transpose of camera Matrix

			camProd = camMatTrans.times(camMat); // Transpose Mat * Camera
			// Mat

			rXMatProd = camMatTrans.times(rXMat); // Transpose Mat * Robot X
			// Mat
			rYMatProd = camMatTrans.times(rYMat); // Transpose Mat * Robot Y
			// Mat

			xFit = camProd.solve(rXMatProd); // Solve for coefficients of
			// robot X line
			yFit = camProd.solve(rYMatProd); // Solve for coefficients of
			// robot Y line

			resultX = xFit.getArray();
			resultY = yFit.getArray();

			mX = resultX[1][0];
			nX = resultX[2][0];
			jX = resultX[0][0];

			mY = resultY[1][0];
			nY = resultY[2][0];
			jY = resultY[0][0];

		} else {
			PanelDemo.theLogger.warn("Processing Data... " + count);
		}
		count++;
	}

	public void showResult() {
		String result = "Curve Equations: \n" + "rX = " + jX + " + " + mX
				+ "*cX + " + nX + "*cY \n" + "rY = " + jY + " + " + mY
				+ "*cX + " + nY + "*cY";

		PanelDemo.theLogger.info(result);
	}

	void getDeviation(double robotX, double robotY, double camX, double camY) {

		double tempY, tempX, errorX, errorY;
		double delta_aX, delta_bX, delta_cX, delta_aY, delta_bY, delta_cY;

		delta_aX = Math.abs(mX - last_aX);
		delta_bX = Math.abs(nX - last_bX);
		delta_cX = Math.abs(jX - last_cX);

		delta_aY = Math.abs(mY - last_aY);
		delta_bY = Math.abs(nY - last_bY);
		delta_cY = Math.abs(jY - last_cY);

		tempX = jX + mX * camX + nX * camY;
		tempY = jY + mY * camX + nY * camY;
		errorX = tempX - robotX;
		errorY = tempY - robotY;

		System.out.println("Error [" + errorX + ", " + errorY + "]");

		System.out.println("DeltaX [" + delta_aX + ", " + delta_bX + ", "
				+ delta_cX + "]");
		System.out.println("DeltaY [" + delta_aY + ", " + delta_bY + ", "
				+ delta_cY + "]");
	}

	// Create Column vector with RobotX Vals
	private double[][] createRobotXArray(List<ObvPoints> obvPointList2) {

		int n = obvPointList2.size();
		double[][] robotX = new double[n][1];
		ObvPoints pt;

		for (int i = 0; i < n; i++) {
			pt = obvPointList2.get(i);
			robotX[i][0] = pt.robotX;
		}
		return robotX;
	}

	// Create Column vector with RobotY Vals

	private double[][] createRobotYArray(List<ObvPoints> obvPointList2) {

		int n = obvPointList2.size();
		double[][] robotY = new double[n][1];
		ObvPoints pt;

		for (int i = 0; i < n; i++) {
			pt = obvPointList2.get(i);
			robotY[i][0] = pt.robotY;
		}
		return robotY;
	}

	// Create nx3 array of the format [1 camX camY]

	private double[][] createCamArray(List<ObvPoints> obvPointList2) {

		int n = obvPointList2.size();
		double[][] camResult = new double[n][3];
		ObvPoints pt;

		for (int i = 0; i < n; i++) {
			pt = obvPointList2.get(i);
			camResult[i][0] = 1;
			camResult[i][1] = pt.camX;
			camResult[i][2] = pt.camY;
		}

		return camResult;
	}

	@Override
	public void fit(double rX, double rY, double camX, double camY) {
		calcFit(rX, rY, camX, camY);
	}

	@Override
	public double[] getResults() {
		return new double[] { mX, nX, jX, mY, nY, jY };
	}

	class ObvPoints {

		private double camX;
		private double camY;
		private double robotX;
		private double robotY;

		public ObvPoints(double rX, double rY, double cX, double cY) {

			this.camX = cX;
			this.camY = cY;
			this.robotX = rX;
			this.robotY = rY;
		}

		public double getCamX() {
			return camX;
		}

		public double getCamY() {
			return camY;
		}

		public double getRobotX() {
			return robotX;
		}

		public double getRobotY() {
			return robotY;
		}
	}

	public static void main(String args[]) {
		DualRegression bestFit = new DualRegression();

		bestFit.calcFit(0.255, -0.08, 0.12858, 0.024);
		bestFit.calcFit(0.404, 0.077, -0.037, -0.118);
		bestFit.calcFit(0.22288, -0.0939, 0.147, 0.047);

	}
}