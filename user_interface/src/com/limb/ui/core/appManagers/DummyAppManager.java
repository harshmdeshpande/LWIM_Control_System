package com.limb.ui.core.appManagers;

import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;

import com.limb.ui.core.devices.DCamera;
import com.limb.ui.core.devices.DRobot;

public class DummyAppManager extends AppManager {

	private DRobot robot;
	private DCamera cam;

	private static boolean t = false;
	private static String name = "APP_DUMMY";

	public DummyAppManager() {
		super(name, DRobot.getRobot());

		this.robot = DRobot.getRobot();
		this.cam = DCamera.getCamera();
	}

	public void performStep() {
		System.out.println(this.robot.getHome());
	}

	public static void main(String[] args) {

		final DummyAppManager app = new DummyAppManager();
		// app.start();

		JFrame f = new JFrame();

		JPanel p = new JPanel();
		p.setLayout(new GridLayout());

		JButton b = new JButton("press");
		b.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				if (t) {
					app.terminate();
				} else {
					// app = new new DummyAppManager(DRobot.ROBOT,
					// DCamera.CAMERA);
					app.start();
				}
				t = !t;
			}

		});

		p.add(b);
		f.getContentPane().add(p);
		f.setVisible(true);
	}
}
