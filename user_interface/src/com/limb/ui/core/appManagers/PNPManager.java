package com.limb.ui.core.appManagers;

import com.limb.comm.client.IConnectionClient;
import com.limb.ui.core.Item;
import com.limb.ui.core.devices.DCamera;
import com.limb.ui.core.devices.DRobot;

public class PNPManager extends AppManager {

	private DRobot robot;
	private DCamera cam;
	private double pickPlane;
	private double placePlane;
	private static String name = "APP_PNP";

	public PNPManager() {
		super(name, DRobot.getRobot(), DCamera.getCamera());

		this.robot = DRobot.getRobot();
		this.cam = DCamera.getCamera();
	}

	public void initialize() {
		System.out.println("Initializing...");
	}

	@Override
	public void performStep() {
		Item vItem = cam.getRandomObject();
		if (vItem != null) {
			double[] placePos;
			if (vItem.getName().equalsIgnoreCase("square")) {
				placePos = getPlacePosition(vItem, -1);
			} else {
				placePos = getPlacePosition(vItem, 1);
			}

			double[] pickPos = getPickPosition(vItem);

			// check if point reachable or not
			pickAndPlace(pickPos, placePos);
		}
	}

	private void pickAndPlace(double[] pickPos, double[] placePos) {
		if (checkReachable(pickPos)) {
			pick(pickPos);
			place(placePos);
		}
	}

	// TODO: combine 'pick' and 'place' methods with gripper = true / false
	private void place(double[] placePos) {
		this.robot.approach(placePos, placePlane);
		System.out.println("gripper OFF");
		this.robot.retract(placePos, placePlane);
	}

	private void pick(double[] pickPos) {
		this.robot.approach(pickPos, pickPlane);
		System.out.println("gripper ON");
		this.robot.retract(pickPos, pickPlane);
	}

	private boolean checkReachable(double[] pickPos) {
		String isReachable = this.robot.solve(pickPos);

		if (isReachable.startsWith(IConnectionClient.ERROR)) {
			return false;
		}

		return true;
	}

	private double[] toRobotCoordinates(double x, double y) {

		// double rX = 0.3651521530783129 - 0.025855316973278877 * x
		// + 0.9585456141137781 * y;
		// double rY = 0.014201581617876527 + 0.9621921797188199 * x
		// - 0.06525558388744486 * y;

		// double rX = panel.mx * x + panel.nx * y + panel.jx;
		// double rY = panel.my * x + panel.ny * y + panel.jy;

		return new double[] { 2, 2 };
	}

	private double[] getPickPosition(Item i) {
		double[] robotP = toRobotCoordinates(i.getX(), i.getY());
		return robotP;
	}

	// TODO: remove hardcode
	private double[] getPlacePosition(Item i, int pos) {
		double[] placeLocation = { 0.152, -0.3 * 1, 0.05, -3.142, 0.0, 0.0 };
		return placeLocation;
	}

}
