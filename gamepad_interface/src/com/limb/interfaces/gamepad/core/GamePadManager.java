package com.limb.interfaces.gamepad.core;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.StringTokenizer;

import javax.swing.JFrame;
import javax.swing.Timer;

import de.hardcode.jxinput.Axis;
import de.hardcode.jxinput.Button;
import de.hardcode.jxinput.Directional;
import de.hardcode.jxinput.JXInputDevice;
import de.hardcode.jxinput.JXInputManager;
import de.hardcode.jxinput.event.JXInputAxisEvent;
import de.hardcode.jxinput.event.JXInputAxisEventListener;
import de.hardcode.jxinput.event.JXInputButtonEvent;
import de.hardcode.jxinput.event.JXInputButtonEventListener;
import de.hardcode.jxinput.event.JXInputDirectionalEvent;
import de.hardcode.jxinput.event.JXInputDirectionalEventListener;
import de.hardcode.jxinput.event.JXInputEventManager;

public abstract class GamePadManager implements ActionListener {

	// TODO: Handle button events as button numbers and NOT names!

	private JXInputDevice joystick;
	private JFrame targetFrame;
	private GamePadListener listener = new GamePadListener();

	public GamePadManager(JFrame targetFrame) {
		this.joystick = JXInputManager.getJXInputDevice(0);
		this.targetFrame = targetFrame;
		if (joystick != null) {
			addListeners();
		}
	}

	private void addListeners() {
		for (int i = 0; i < joystick.getMaxNumberOfAxes(); i++) {
			Axis axis = joystick.getAxis(i);
			if (axis != null) {
				JXInputEventManager.addListener(listener, axis, 0.1);
			}
		}

		for (int i = 0; i < joystick.getMaxNumberOfButtons(); i++) {
			Button btn = joystick.getButton(i);
			if (btn != null) {
				JXInputEventManager.addListener(listener, btn);
			}
		}

		for (int i = 0; i < joystick.getMaxNumberOfDirectionals(); i++) {
			Directional dir = joystick.getDirectional(i);
			if (dir != null) {
				JXInputEventManager.addListener(listener, dir);
			}
		}

	}

	public void startPad() {
		System.out.println("Starting Gamepad Manager");
		targetFrame.setVisible(true);
		new Timer(50, this).start();
	}

	@Override
	public void actionPerformed(ActionEvent arg0) {
		JXInputManager.updateFeatures();
	}

	public void axisChanged(JXInputAxisEvent e) {

	}

	public void axisChanged(String axisName, double axisValue) {

	}

	public void buttonChanged(JXInputButtonEvent e) {

	}

	public void buttonChanged(int buttonID, int state) {

	}

	public void directionChanged(JXInputDirectionalEvent e) {

	}

	public void directionChanged(String direction, int status) {
	}

	public void directionChanged(double x, double y) {
	}

	class GamePadListener implements JXInputAxisEventListener,
			JXInputButtonEventListener, JXInputDirectionalEventListener {

		// private int prevDirection = 0;

		@Override
		public void changed(JXInputAxisEvent e) {
			axisChanged(e);
			axisChanged(processAxis(e.getAxis().getName()), processValue(e
					.getAxis().getValue()));
		}

		@Override
		public void changed(JXInputButtonEvent e) {
			buttonChanged(e);
			int state = e.getButton().getState() ? 1 : 0;
			int buttonID = getButtonID(processButton(e.getButton().getName()));
			buttonChanged(buttonID, state);
		}

		@Override
		public void changed(JXInputDirectionalEvent e) {
			directionChanged(e);
			directionChanged("T" + (e.getDirectional().getDirection()) / 100,
					(int) e.getDirectional().getValue());

			double direction = e.getDirectional().getDirection() / 100.0;
			double state = e.getDirectional().getValue();

			double x = state * Math.sin(Math.toRadians(direction));
			double y = state * Math.cos(Math.toRadians(direction));

			if (Math.abs(x) < 0.0000000001) {
				x = 0;
			}

			if (Math.abs(y) < 0.0000000001) {
				y = 0;
			}

			directionChanged(x, y);
		}

		private String processAxis(String inputName) {
			StringTokenizer st = new StringTokenizer(inputName);
			return st.nextToken().substring(0, 1)
					+ st.nextToken().substring(0, 1);
		}

		private String processButton(String buttonName) {
			StringTokenizer st = new StringTokenizer(buttonName);
			st.nextToken();
			return "B" + st.nextToken();
		}

		private double processValue(double val) {
			return Math.round(val);
		}

	}

	public static String getButtonName(int buttonID) {
		switch (buttonID) {
		case 1:
			return IGamePadCommands.Button1;
		case 2:
			return IGamePadCommands.Button2;
		case 3:
			return IGamePadCommands.Button3;
		case 4:
			return IGamePadCommands.Button4;
		case 5:
			return IGamePadCommands.Button5;
		case 6:
			return IGamePadCommands.Button6;
		case 7:
			return IGamePadCommands.Button7;
		case 8:
			return IGamePadCommands.Button8;
		case 9:
			return IGamePadCommands.ButtonSelect;
		case 10:
			return IGamePadCommands.ButtonStart;
		case 11:
			return IGamePadCommands.Button11;
		case 12:
			return IGamePadCommands.Button12;
		}
		
		return null;
	}

	public static int getButtonID(String b) {
		if (b.equalsIgnoreCase(IGamePadCommands.Button1))
			return 1;

		if (b.equalsIgnoreCase(IGamePadCommands.Button2))
			return 2;

		if (b.equalsIgnoreCase(IGamePadCommands.Button3))
			return 3;

		if (b.equalsIgnoreCase(IGamePadCommands.Button4))
			return 4;

		if (b.equalsIgnoreCase(IGamePadCommands.Button5))
			return 5;

		if (b.equalsIgnoreCase(IGamePadCommands.Button6))
			return 6;

		if (b.equalsIgnoreCase(IGamePadCommands.Button7))
			return 7;

		if (b.equalsIgnoreCase(IGamePadCommands.Button8))
			return 8;

		if (b.equalsIgnoreCase(IGamePadCommands.ButtonSelect))
			return 9;

		if (b.equalsIgnoreCase(IGamePadCommands.ButtonStart))
			return 10;

		if (b.equalsIgnoreCase(IGamePadCommands.Button11))
			return 11;

		if (b.equalsIgnoreCase(IGamePadCommands.Button12))
			return 12;

		return -1;
	}
}
