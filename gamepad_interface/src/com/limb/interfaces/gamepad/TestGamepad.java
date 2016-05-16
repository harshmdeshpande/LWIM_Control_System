package com.limb.interfaces.gamepad;

import javax.swing.JFrame;

import com.limb.interfaces.gamepad.core.GamePadManager;

public class TestGamepad {

	public static void main(String[] args) {
		
		JFrame testFrame = new JFrame();
		GamePadManager gpm = new GamePadManager(testFrame){};
		gpm.startPad();
	}
}
