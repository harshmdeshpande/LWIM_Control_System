package com.limb.simulation.control;

import javax.robotics.engine.robots.Robot3D;
import javax.robotics.vecmath.RVector;

public class RobotMotionManager {

	private static final int STEPS = 1000;
	
	private static final RobotMotionManager instance = new RobotMotionManager();
	
//	private FastList<IRobotListener> robotL = new FastList<IRobotListener>();
	
	private RobotMotionManager() {};
	
	public static RobotMotionManager getInstance() {
		return instance;
	}

	public void setRobotP(Robot3D robot, double[] jp, boolean animate) {
		//TODO: shud the robot animate in 3D?
		//animate = false;
		
		if(!animate) {
			robot.setQ3D(jp);
			return;
		}
		
		RVector initialPos = robot.getQ();
		RVector finalPos = new RVector(jp);
		
		RVector range = new RVector(finalPos);
		range.sub(initialPos);
				
		RVector pos = new RVector(6);
		for(int k = 0; k < STEPS; k++) {
			for(int i = 0; i < initialPos.getSize(); i++) {
				
				pos.setElement(i, 
						initialPos.getElement(i) + range.getElement(i)/STEPS * k);
			
			}
			//System.out.println("Position: \n" + pos);
			robot.setQ3D(pos);
		}
	}


}
