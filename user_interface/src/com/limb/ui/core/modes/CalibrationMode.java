package com.limb.ui.core.modes;

import com.limb.ui.core.appManagers.AppManager;

public class CalibrationMode extends Mode {

	private static String name = "MODE_CALIB";
	
	public CalibrationMode(AppManager... appManager) {
		super(name, appManager);
		
	}

}
