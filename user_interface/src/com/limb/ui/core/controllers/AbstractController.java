package com.limb.ui.core.controllers;

import com.limb.ui.core.IJSController;
import com.limb.ui.panels.TabbedPanel;

public class AbstractController implements IJSController {

	protected TabbedPanel panel;
	private String name;
	private boolean eeActuate = false;
	
	private void toggleEE() {
		String msg = eeActuate ? "open" : "close";
		panel.sendMessage(msg);
		eeActuate = !eeActuate;
	}

	public AbstractController(TabbedPanel panel) {
		this.panel = panel;
		this.name = "Generic Controller";
	}

	@Override
	public String getName() {
		return name;
	}
	
	@Override
	public void setName(String name) {
		this.name = name;
	}
	
	public String readyRobot() {
		return panel.sendMessage("ready");
	}
	
	public String halt() {
		return panel.sendMessage("halt");
	}
	
	@Override
	public void handleAxisChanged(String axisName, double axisValue) {
	}

	@Override
	public void handleButtonChanged(int buttonID, int state) {
		if(buttonID == 4 && state == 0) {
			toggleEE();
		}
	}

	@Override
	public void handleDirectionChanged(double x, double y) {
	}

}
