package com.limb.comm.client;

public class RobotControlClient implements IConnectionClient {

	private final CommandClient cmdClient;
	private final FeedbackClient fbClient;

	public RobotControlClient() {
		cmdClient = new CommandClient();
		fbClient = new FeedbackClient();
	}

	public void hookFeedbackProcessor(IFeedbackProcessor fp) {
		fbClient.hookFeedbackProcessor(fp);
	}

	public boolean connect(String IP, int cmdPort, String fbIP, int fbPort) {
		return fbClient.connect(fbIP, fbPort) && cmdClient.connect(IP, cmdPort);
	}

	public boolean disconnect() {
		return cmdClient.disconnect() && fbClient.disconnect();
	}

	public void startFeedbackLoop() {
		fbClient.startFeedbackLoop();
	}

	public String sendMessage(String message) {
		return cmdClient.sendMessage(message);
	}

	public boolean isConnected() {
		return cmdClient.isConnected();// && fbClient.isConnected();
	}
}
