package com.limb.simulation;

import java.io.File;
import java.io.IOException;
import java.net.URI;

import javax.media.j3d.Transform3D;
import javax.media.j3d.TransformGroup;
import javax.robotics.engine.robots.DHRobot3D;
import javax.robotics.engine.robots.Robot3D;
import javax.robotics.swing.RobotFrame;
import javax.swing.JFrame;
import javax.vecmath.Matrix4d;

import org.apache.xmlbeans.XmlException;

import com.limb.simulation.control.KinematicsEngine;

public class Simulation extends RobotFrame {

	private static final long serialVersionUID = 1L;

	private static final Simulation INSTANCE = new Simulation();

	private KinematicsEngine kineEngine;

	private Simulation() {
		super();
		try {
			createSimFrame();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static Simulation getInstance() {
		return INSTANCE;
	}
	
	private void createSimFrame() throws XmlException, IOException {
		final URI file = new File("conf/robot.xml").toURI();

		// create robot
		final Robot3D robot = new DHRobot3D(file);
		// double[] qmax = robot.getRobotConfiguration().getQMax();
		// double[] qmin = robot.getRobotConfiguration().getQMin();
		//
		// // for (int i = 0; i < qmin.length; i++) {
		// // System.out.println("Joint " + (i + 1) + ": ["
		// // + Math.toDegrees(qmin[i]) + ", " + Math.toDegrees(qmax[i])
		// // + "]");
		////		}

		// add the robot to the window
		addRobotToDefaultConfiguration(robot);

		// set the nice viewpoint
		setViewPoint();

		// set window size
		setSize(600, 400);
		
		//init the sim backend
		kineEngine = new KinematicsEngine(robot);
		
		this.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
	}
	

	private void setViewPoint() {
		TransformGroup vt = this.getSimpleUniverse().getViewingPlatform()
				.getViewPlatformTransform();
		Transform3D viewPoint = new Transform3D();
		vt.getTransform(viewPoint);

		/*
		 * -0.7232817299842463, -0.4199746345367401, 0.5481649801079255,
		 * 2.2114298754276924 0.6904040370360589, -0.45627525755379117,
		 * 0.5613868140493946, 2.0917256339675463 0.01434589541657267,
		 * 0.7844961412842949, 0.6199677407694233, 2.46418943876771 0.0, 0.0,
		 * 0.0, 1.0
		 */

		Matrix4d m = new Matrix4d();
		m.m00 = -0.7232817299842463;
		m.m01 = -0.4199746345367401;
		m.m02 = 0.5481649801079255;
		m.m03 = 2.2114298754276924;
		m.m10 = 0.6904040370360589;
		m.m11 = -0.45627525755379117;
		m.m12 = 0.5613868140493946;
		m.m13 = 2.0917256339675463;
		m.m20 = 0.01434589541657267;
		m.m21 = 0.7844961412842949;
		m.m22 = 0.6199677407694233;
		m.m23 = 2.46418943876771;
		m.m30 = 0.0;
		m.m31 = 0.0;
		m.m32 = 0.0;
		m.m33 = 1.0;

		viewPoint.set(m);
		vt.setTransform(viewPoint);
	}

	public static void launch() {
		INSTANCE.setVisible(true);
	}
	
	public static void main(String[] args) {
		Simulation.launch();
	}

	public boolean isOpen() {
		return isVisible();
	}

	public void updateSim(int nodeID, double q) {
		kineEngine.updateSim(nodeID, q);
	}
}
