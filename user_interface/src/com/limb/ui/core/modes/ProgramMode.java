package com.limb.ui.core.modes;

import com.limb.ui.core.appManagers.AppManager;

public class ProgramMode extends Mode {

	private static String name = "MODE_PROGRAM";

	public ProgramMode(AppManager... appManager) {
		super(name, appManager);
	}

}
