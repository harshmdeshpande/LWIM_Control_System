package com.limb.ui.core.appManagers;

import com.limb.ui.core.devices.DRobot;

public class HomingAppManager extends AppManager {

	private static String name = "APP_HOMING";
	private DRobot robot;

	public HomingAppManager() {
		super(name, DRobot.getRobot());

		this.robot = DRobot.getRobot();
	}

	@Override
	public void performStep() {

	}

}
