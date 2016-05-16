package com.limb.ui.core;

public interface IJSController {

	void handleDirectionChanged(double x, double y);

	void handleButtonChanged(int buttonID, int state);

	void handleAxisChanged(String axisName, double axisValue);

	String getName();
	
	void setName(String name);

}
