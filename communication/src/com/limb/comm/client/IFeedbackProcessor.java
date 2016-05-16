package com.limb.comm.client;

public interface IFeedbackProcessor {
	
	void processFeedback(int nodeID, AxisData d, String status);
}
