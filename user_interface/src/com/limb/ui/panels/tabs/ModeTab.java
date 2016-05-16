package com.limb.ui.panels.tabs;

import java.awt.Component;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JPanel;

import com.limb.comm.client.IConnectionClient;
import com.limb.ui.core.IJSController;
import com.limb.ui.core.appManagers.AppManager;
import com.limb.ui.core.modes.Mode;
import com.limb.ui.panels.TabbedPanel;
import com.limb.ui.panels.helpers.UIManager;

public class ModeTab extends JPanel {

	private static final long serialVersionUID = 1L;
	private JButton btn;
	private String btnLabel;

	protected TabbedPanel panel;
	private IJSController activeController;
	private Mode mode;
	private String modeName;
	private AppManager appManager;

	public ModeTab(String name, TabbedPanel parent) {
		this.panel = parent;
		createEnterModeButton(name);
		// init();
	}

	public ModeTab(Mode mode) {
		this.mode = mode;
		this.modeName = mode.getName();

		createEnterModeButton(mode.getName());
		createAppButtons(mode.getAppManagers());

		enableChildren(false);
		// init();
	}

	private void createEnterModeButton(String name) {
		this.btnLabel = "Enter " + name;

		this.setLayout(new FlowLayout(FlowLayout.LEFT));
		btn = new JButton();
		btn.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				enterMode();
			}

		});
		setStatus(btnLabel);
		this.add(btn);
	}

	private void createAppButtons(AppManager[] appManagers) {
		for (int i = 0; i < appManagers.length; i++) {
			final AppManager m = appManagers[i];
			JButton b = new JButton(appManagers[i].getAppName());
			b.addActionListener(new ActionListener() {

				@Override
				public void actionPerformed(ActionEvent arg0) {
					handleApp(m);
				}

			});
			this.add(b);
		}
	}

	protected void handleApp(AppManager appManager) {
		if (appManager.init()) {
			appManager.start();
		}
	}

	protected void setStatus(String status) {
		if (btn != null) {
			btn.setText(status);
		}
	}

	public void enterMode() {
		enableChildren(true);
		this.mode.init();
	}

	public void exitMode() {
		enableChildren(false);
	}

	protected boolean isOK(String response) {
		if (response == null) {
			return false;
		}
		return !response.startsWith(IConnectionClient.ERROR);
	}

	public IJSController getActiveController() {
		return activeController;
	}

	public void setActiveController(IJSController c) {
		this.activeController = c;
	}

	protected void enableChildren(boolean enable) {
		int children = this.getComponentCount();

		for (int i = 0; i < children; i++) {
			Component c = this.getComponent(i);
			if (c != btn) {
				UIManager.enableComponent(c, enable);
			}
		}
	}

	public String getModeName() {
		return modeName;
	}
}
