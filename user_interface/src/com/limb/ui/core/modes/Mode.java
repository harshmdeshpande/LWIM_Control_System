package com.limb.ui.core.modes;

import com.limb.ui.core.appManagers.AppManager;

public abstract class Mode {

	private AppManager[] appManager;
	private String modeName;

	public Mode(String modeName, AppManager... appManager) {
		this.appManager = appManager;
		this.modeName = modeName;
	}

	public void init() {
		for (int i = 0; i < appManager.length; i++) {
			if (appManager[i].init()) {
				appManager[i].start();
			}
		}

		// appManager[0].start();

	}

	public String getName() {
		return this.modeName;
	}

	public AppManager[] getAppManagers() {
		return appManager;
	}

	public AppManager getAppManager(int index) {
		return appManager[index];
	}

}
