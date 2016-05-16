package com.limb.ui.core.appManagers;

import com.limb.ui.core.devices.DRobot;

public class InsertionManager extends AppManager {

	private DRobot robot;
	private static String name = "APP_INSERTION";

	public InsertionManager() {
		super(name, DRobot.getRobot());

		this.robot = DRobot.getRobot();
	}

	@Override
	public void performStep() {
	}

	public static void main(String[] args) {
		new InsertionManager().start();
	}

}
