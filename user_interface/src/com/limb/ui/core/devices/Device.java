package com.limb.ui.core.devices;

import com.limb.comm.client.CommandClient;
import com.limb.ui.PanelDemo;

public abstract class Device {

	private int port;
	private String IP;
	private String DeviceID;
	private boolean isActive;

	private final CommandClient client = new CommandClient();

	public Device(String IP, int port, String DeviceID) {
		this.IP = IP;
		this.port = port;
		this.DeviceID = DeviceID;
	}

	public boolean connect() {
		if (client == null) {
			return false;
		}
		if (!client.isConnected()) {
			return client.connect(IP, port);
		}
		return true;
	}

	public String sendMessage(String command) {
		if (client == null) {
			return null;
		}

		if (!client.isConnected()) {
			if (!client.connect(IP, port)) {
				PanelDemo.theLogger.warn("Could not connect to " + getID());
				// System.out.println("TERMINATED!!!!!!!!");
				setActive(false);
				client.disconnect();

				return null;
			}
		}

		return this.DeviceID + " : " + client.sendMessage(command);
	}

	public String getID() {
		return this.DeviceID;
	}

	public static Device getInstance() {
		return null;
	}

	public void setActive(boolean isActive) {
		this.isActive = isActive;
	}

	public boolean isActive() {
		return isActive;
	}

	public boolean isConnected() {
		return client.isConnected();
	}

}
