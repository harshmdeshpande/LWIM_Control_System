package com.limb.ui.core.modes;

import com.limb.ui.core.appManagers.AppManager;
import com.limb.ui.core.appManagers.DummyAppManager;
import com.limb.ui.core.appManagers.HomingAppManager;
import com.limb.ui.core.appManagers.InsertionManager;
import com.limb.ui.core.appManagers.PNPManager;

public class ModeManager {

	private CalibrationMode calibrationMode;
	private ProgramMode programMode;
	private ExecutionMode executionMode;

	private Mode currentMode;

	public ModeManager() {

		DummyAppManager dummyAppManager = new DummyAppManager();
		dummyAppManager.setConstraint(AppManager.CYCLES, 10);

		this.calibrationMode = new CalibrationMode(new HomingAppManager());
		this.programMode = new ProgramMode();
		this.executionMode = new ExecutionMode(dummyAppManager,
				new InsertionManager());

		this.currentMode = this.calibrationMode;
	}

	public void startStateMachine() {
		this.executionMode.init();
	}

	public CalibrationMode getCalibrationMode() {
		return calibrationMode;
	}

	public ProgramMode getProgramMode() {
		return programMode;
	}

	public ExecutionMode getExecutionMode() {
		return executionMode;
	}
}
