package com.limb.ui.panels.helpers;

import java.awt.Component;
import java.awt.Container;

public class UIManager {

	public static void enableComponent(Component c, boolean enable) {
		if (c instanceof Container) {
			enableContainer((Container) c, enable);
		} else {
			c.setEnabled(enable);
		}

	}

	public static void enableContainer(Container c, boolean enable) {
		Component[] children = c.getComponents();
		for (int i = 0; i < children.length; i++) {
			enableComponent(children[i], enable);
		}
		
		c.setEnabled(enable);
	}
}
