package com.limb.ui.core.devices;

import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;

import com.limb.comm.client.IConnectionClient;
import com.limb.ui.core.Item;

public class DCamera extends Device {

	private static final String GET_OBJECT = "get";
	private static final String NOT_FOUND = "notfound";

	private static final DCamera CAMERA = new DCamera("localhost", 2222,
			"Camera");

	private DCamera(String IP, int port, String DeviceID) {
		super(IP, port, DeviceID);
	}

	public Item getRandomObject() {
		String s = NOT_FOUND;

		while (isActive() && s.equalsIgnoreCase(NOT_FOUND)) {
			s = sendMessage(GET_OBJECT);

			if (s == null || s.startsWith(IConnectionClient.ERROR)) {
				return null;
			}
		}

		if (!isActive()) {
			return null;
		}

		List<Item> itemList = new ArrayList<Item>();

		StringTokenizer objListTokenizer = new StringTokenizer(s, "#");
		while (objListTokenizer.hasMoreTokens()) {
			String str = objListTokenizer.nextToken();
			itemList.add(new Item(str));
		}

		if (itemList.isEmpty()) {
			return null;
		}

		return itemList.get(0);
	}

	public static DCamera getCamera() {
		return CAMERA;
	}
}
