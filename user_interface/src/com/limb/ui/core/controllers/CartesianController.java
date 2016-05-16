package com.limb.ui.core.controllers;

import com.limb.interfaces.gamepad.core.IGamePadCommands;
import com.limb.ui.panels.TabbedPanel;

public class CartesianController extends AbstractController {

	private static final double rFactor = 2 * Math.PI;
	private double jogVelo = 0.02;
	private double[] jogSpeedVector = new double[6];

	public CartesianController(TabbedPanel panel) {
		super(panel);

		for (int i = 0; i < 6; i++) {
			jogSpeedVector[i] = 0;
		}
		
		setName("Cartesian Controller");
	}

	private void setJogSpeed(double jv) {
		jogVelo += jv;
		System.out.println(jogVelo);
	}

	protected void setJogVelo(int index, double state) {
		System.out.println(state);
		jogSpeedVector[index - 1] = jogVelo * state;
		sendJogMessage();
	}

	private void sendJogMessage() {
		String msg = "setjogvelo ";
		for (int i = 0; i < 6; i++) {
			msg += jogSpeedVector[i] + " ";
		}

		System.out.println(msg);
		panel.sendMessage(msg);
	}

	@Override
	public void handleButtonChanged(int buttonID, int state) {
		super.handleButtonChanged(buttonID, state);

		switch (buttonID) {
		case 5:
		case 6:
			setJogSpeed(-0.005);
			break;

		case 7:
		case 8:
			setJogSpeed(0.005);
			break;

		default:
			break;
		}
	}

	public void handleAxisChanged(String axisName, double axisValue) {
		if (axisName.equalsIgnoreCase(IGamePadCommands.YAxis)) {
			setJogVelo(5, axisValue * rFactor); // roty
		}
		// Note the switch - nothing really to do here
		if (axisName.equalsIgnoreCase(IGamePadCommands.XAxis)) {
			setJogVelo(6, axisValue * rFactor); // rotz
		}
		if (axisName.equalsIgnoreCase(IGamePadCommands.ZAxis)) {
			setJogVelo(4, axisValue * rFactor); // rotx
		}
		if (axisName.equalsIgnoreCase(IGamePadCommands.ZRot)) {
			setJogVelo(3, -axisValue); // translation z
		}
	}

	public void handleDirectionChanged(double x, double y) {
		setJogVelo(2, x);
		setJogVelo(1, y);
	}

}
