package com.limb.ui.core.appManagers;

import com.limb.ui.core.devices.Device;

public abstract class AppManager extends Thread {

	public static String TIME = "Time";
	public static String CYCLES = "Cycles";
	private String CONSTRAINT = AppManager.CYCLES; // default constraint

	private double time = 10.0;
	private int cycles = 1;

	private boolean ready = true;
	private Device[] device;
	private String appName;

	public AppManager(String appName, Device... device) {
		this.device = device;
		this.appName = appName;
	}

	public boolean init() {
		ready = true;
		for (int i = 0; i < device.length; i++) {
			if (!device[i].isConnected()) {
				boolean b = device[i].connect();
				ready = ready & b;
				if (!b) {
					System.out.println("Could not connect to "
							+ device[i].getID() + ". Please check connection.");
				} else {
					device[i].setActive(true);
					System.out.println("Connected to " + device[i].getID());
				}
			} else {
				boolean b = device[i].isActive();
				System.out.println(device[i].getID() + " " + b + " "
						+ this.getAppName());
				ready = ready & !b;
				if (b) {
					System.out.println(device[i].getID()
							+ " already in use. Please try after sometime.");
				}
			}
		}
		return ready;
	}

	@Override
	public void run() {
		System.out.println("Starting " + this.getAppName());
		// while (ready && isActive()) {
		// performStep();
		// try {
		// Thread.sleep(1000);
		// } catch (InterruptedException e) {
		// e.printStackTrace();
		// }
		// }

		if (this.CONSTRAINT.equalsIgnoreCase(AppManager.TIME)) {
			runForDuration();
		} else if (this.CONSTRAINT.equalsIgnoreCase(AppManager.CYCLES)) {
			System.out.println("cycles");
			rundForCycles();
		}

		terminate();
	}

	private void rundForCycles() {
		for (int i = 0; i < this.cycles; i++) {
			System.out.println(i);
			step();
		}
	}

	private void runForDuration() {
		// TODO: init timer
	}

	private void step() {
		if (ready && isActive()) {
			performStep();
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}

	}

	public void terminate() {
		System.out.println(this.getAppName() + " terminated!");
		ready = false;
		this.interrupt();
		setActive(false);

	}

	private void setActive(boolean b) {
		for (int i = 0; i < device.length; i++) {
			device[i].setActive(b);
		}
	}

	public abstract void performStep();

	public boolean isActive() {
		for (int i = 0; i < device.length; i++) {
			if (!device[i].isActive()) {
				return false;
			}
		}
		return true;
	}

	public String getAppName() {
		return this.appName;
	}

	public Device[] getDevices() {
		return device;
	}

	public void setConstraint(String constraint, double val) {
		this.CONSTRAINT = constraint;
		if (this.CONSTRAINT.equalsIgnoreCase(AppManager.TIME)) {
			this.time = val;
		} else if (this.CONSTRAINT.equalsIgnoreCase(AppManager.CYCLES)) {
			this.cycles = (int) val;
		}
	}

}
