package com.limb.ui.core;

import java.util.StringTokenizer;

public class Item {

	private String objName;
	private double x;
	private double y;

	public Item(String s) {
		try {
			StringTokenizer st = new StringTokenizer(s, " ");

			this.objName = st.nextToken();
			this.x = Double.parseDouble(st.nextToken());
			this.y = Double.parseDouble(st.nextToken());
		} catch (Exception e) {
			// TODO: handle exception
		}
	}

	@Override
	public String toString() {
		return this.objName + " ( " + x + " " + y + " )";
	}

	public String getName() {
		return this.objName;
	}

	public double getX() {
		return this.x;
	}

	public double getY() {
		return this.y;
	}

}
