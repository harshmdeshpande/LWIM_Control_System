package com.limb.ui.core.devices;

public class DRobot extends Device {

	public static final String GOHOME = "gohome ";
	public static final String READY = "ready ";
	public static final String KINE = "kine ";
	public static final String GETHOMESTAT = "gethomestat ";
	public static final String GETHOME = "gethome ";
	public static final String GETPOS = "getjpos ";
	public static final String GETTICKS = "getticks ";
	public static final String GETRPM = "getrpm ";
	public static final String HALT = "halt ";
	public static final String SETHOME = "sethome ";

	// public static final String = "";
	// public static final String = "";
	// public static final String = "";
	// public static final String = "";

	private static final DRobot ROBOT = new DRobot("localhost", 2000, "Robot");

	private DRobot(String IP, int port, String deviceID) {
		super(IP, port, deviceID);
	}

	public void goHome() {
		sendMessage(GOHOME);
	}

	public void ready() {
		sendMessage(READY);
	}

	public void getKinePosition() {
		sendMessage(KINE);
	}

	// FEEDBACK DATA
	public void getHomeStatus() {
		sendMessage(GETHOMESTAT);
	}

	public String getHome() {
		return sendMessage(GETHOME);
	}

	public void getPosition() {
		sendMessage(GETPOS);
	}

	public void getTicks() {
		sendMessage(GETTICKS);
	}

	public void getRPM() {
		sendMessage(GETRPM);
	}

	public void halt() {
		sendMessage(HALT);
	}

	// CALIB MODE
	public void setHome(Integer axis) {
		String command = SETHOME;
		if (axis != null) {
			command = command + axis;
		}
		sendMessage(command);
	}

	public void moveRPM() {
	}

	public void incrementRPM() {

	}

	public void moveJoiintInDirection() {

	}

	// JOG MODE
	public void setPosition() {

	}

	// EXECUTE MODE
	public String solve(double[] pickPos) {
		return "reachable";
	}

	public void moveToPosition() {

	}

	public void followFromCurrentPosition() {

	}

	public void followPoints() {

	}

	public void followFile() {

	}

	public void followTmp(double... pts) {
		System.out.println(pts);
		for (int i = 0; i < pts.length; i++) {
			System.out.println(pts[i]);
		}
	}
	
	public static DRobot getRobot(){
		return ROBOT;
	}

	public void approach(double[] pickPos, double pickPlane) {
		
	}

	public void retract(double[] pickPos, double pickPlane) {
		
	}

}
