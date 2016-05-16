package com.limb.ui.core.controllers;

import com.limb.interfaces.gamepad.core.IGamePadCommands;
import com.limb.ui.panels.TabbedPanel;

public class JointController extends AbstractController {

	public JointController(TabbedPanel panel) {
		super(panel);
		setName("Joint Controller");
	}

	public void driveAxis(int dir) {
		if (panel.getActiveAxis() > 0) {
			panel.sendMessage("movejdir " + panel.getActiveAxis() + " " + dir);
		}
	}

	public void setSpeed(int sPC) {
		if (panel.getActiveAxis() > 0) {
			panel.sendMessage("incrpm " + panel.getActiveAxis() + " " + sPC);

		}
	}

	@Override
	public void handleAxisChanged(String axisName, double axisValue) {
		if (axisName.equalsIgnoreCase(IGamePadCommands.ZRot)) {
			halt();
			int axis = (int) (panel.getActiveAxis() + axisValue);
			if (axis < 1)
				axis = 1;

			panel.setActiveAxis(axis);
		} else if (axisName.equalsIgnoreCase(IGamePadCommands.YAxis)) {
			driveAxis((int) axisValue);
		}
	}

	@Override
	public void handleButtonChanged(int buttonID, int state) {
		super.handleButtonChanged(buttonID, state);
	}

	@Override
	public void handleDirectionChanged(double x, double y) {
	}

}
