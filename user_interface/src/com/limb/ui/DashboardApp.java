package com.limb.ui;

import javax.swing.BoxLayout;
import javax.swing.JFrame;

import com.limb.comm.client.AxisData;
import com.limb.comm.client.FeedbackClient;
import com.limb.comm.client.IFeedbackProcessor;
import com.limb.ui.dashboards.Cockpit;
import com.limb.ui.panels.ConnectionPanel;

public class DashboardApp {

	private FeedbackClient fbClient;

	public static void main(String[] args) {
		new DashboardApp().start();
	}

	private void start() {
		// Schedule a job for the event-dispatching thread:
		// creating and showing this application's GUI.
		javax.swing.SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				createAndShowGUI();
			}
		});
	}

	protected void createAndShowGUI() {
		ConnectionPanel cPanel = new ConnectionPanel("localhost", 2001) {
			private static final long serialVersionUID = 1L;

			@Override
			protected void handleConnectionButtonClicked() {
				this.setConnected(connect(getIP(), getPort()));
			}
		};

		final Cockpit dashBoard = new Cockpit();

		fbClient = new FeedbackClient();
		fbClient.hookFeedbackProcessor(new IFeedbackProcessor() {
			@Override
			public void processFeedback(int nodeID, AxisData d, String status) {
				dashBoard.updatePanel(nodeID, d);
			}

		});

		JFrame dashFrame = new JFrame();
		dashFrame.setLayout(new BoxLayout(dashFrame.getContentPane(),
				BoxLayout.Y_AXIS));
		dashFrame.add(cPanel);
		dashFrame.add(dashBoard);

		dashFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		dashFrame.pack();
		dashFrame.setVisible(true);
	}

	protected boolean connect(String ip, int port) {
		if (fbClient.isConnected()) {
			disconnect();
			return false;
		}

		if (fbClient.connect(ip, port)) {
			fbClient.startFeedbackLoop();
			return true;
		}

		return false;
	}

	private void disconnect() {
		fbClient.disconnect();
	}
}
