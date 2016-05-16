package com.limb.simulation.control;

import javax.robotics.engine.robots.Robot3D;

public class KinematicsEngine {

	private static final long serialVersionUID = 1L;

	private Robot3D robot;
	double d;

	public KinematicsEngine(Robot3D robot) {
		this.robot = robot;
	}

	public void updateSim(int nodeID, double q) {
		// System.out.println(this.getClass().getName() + " " + q);
		robot.setQ3DOnly(q, nodeID);
	}

}
