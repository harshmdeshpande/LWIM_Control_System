package com.limb.simulation;

/*
 * Demo3D.java -
 * Copyright (c) 2005 Carmine Lia
 * e-mail:      carmine.lia@libero.it
 * web-address: http://www.intermedia.sa.it/lia

 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */
import java.awt.BorderLayout;
import java.io.IOException;
import java.net.URISyntaxException;

import javax.robotics.engine.robots.DHRobot3D;
import javax.robotics.engine.robots.Robot3D;
import javax.robotics.engine.trajectory.Spline;
import javax.robotics.swing.JointSliderPanel;
import javax.robotics.swing.KinematicsPanel;
import javax.robotics.swing.RobotFrame;
import javax.swing.JFrame;

import org.apache.xmlbeans.XmlException;

/**
 * Robot3D class demonstration. The application shows how to create a 3D
 * representation of robot.
 * 
 * @author Carmine Lia
 * 
 */
public class Demo3D extends RobotFrame {

	/**
	 * 
	 */
	private static final long serialVersionUID = 3906651907677567288L;

	private final JFrame sliderWindow;

	// robotModel
	private final Robot3D robot;

	private Demo3D() throws XmlException, IOException, URISyntaxException {
		super();

		robot = new DHRobot3D("conf/robot.xml");

		// add the robot to the window
		addRobotToDefaultConfiguration(robot);

		System.out.println("dof " + robot.getDof());
		System.out.println("avail dof " + robot.getAvailableDof());
		System.out.println("qmin " + robot.getQMin());
		System.out.println("qmax " + robot.getQMax());
		System.out.println("qoffs " + robot.getQOffset());

		// add the kinematics panel to the window
		add(new KinematicsPanel(robot), BorderLayout.EAST);
		setTitle(robot.getRobotConfiguration().getName());
		setSize(800, 600);
		setVisible(true);

		// show the slider panel to move the joints
		sliderWindow = new JFrame("Joints Window");
		sliderWindow.add(new JointSliderPanel(robot));
		sliderWindow.pack();
		sliderWindow.setVisible(true);

	}

	public static void main(String[] args) throws XmlException, IOException,
			URISyntaxException {
		new Demo3D();
	}
}