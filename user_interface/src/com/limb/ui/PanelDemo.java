package com.limb.ui;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.File;
import java.io.IOException;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFrame;

import org.apache.log4j.Logger;

import com.limb.comm.client.AxisData;
import com.limb.comm.client.IFeedbackProcessor;
import com.limb.comm.client.RobotControlClient;
import com.limb.interfaces.gamepad.core.GamePadManager;
import com.limb.simulation.Simulation;
import com.limb.ui.dashboards.Cockpit;
import com.limb.ui.panels.CommandPanel;
import com.limb.ui.panels.ConnectionPanel;
import com.limb.ui.panels.LogConsolePanel;
import com.limb.ui.panels.TabbedPanel;
import com.limb.ui.panels.helpers.UIManager;

public class PanelDemo {

	public static final Logger theLogger = Logger.getLogger(PanelDemo.class);

	private static final String COMMAND_FILENAME = "commands.dat";
	private RobotControlClient rcc;

	protected String IP;
	protected int port;

	private CommandPanel commandPanel;
	private TabbedPanel tabbedPanel;

	public static void main(String[] args) {
		new PanelDemo().start();
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

	public void handleExit() {
		File f = new File(COMMAND_FILENAME);
		if (!f.exists()) {
			try {
				f.createNewFile();
			} catch (IOException e) {
				e.printStackTrace();
				return;
			}
		}

		commandPanel.saveHistoryToFile(f);
	}

	protected void createAndShowGUI() {
		JFrame coreFrame = new JFrame();

		rcc = new RobotControlClient();

		final Cockpit dashBoard = new Cockpit();

		tabbedPanel = new TabbedPanel(rcc, coreFrame) {
			private static final long serialVersionUID = 1L;

			@Override
			protected void selectAxis(int axis) {
				dashBoard.highlightNode(axis);
			}
		};

		ConnectionPanel cPanel = new ConnectionPanel("localhost", 2000) {
			private static final long serialVersionUID = 1L;

			@Override
			protected void handleConnectionButtonClicked() {
				PanelDemo.this.IP = getIP();
				PanelDemo.this.port = getPort();
				boolean connected = connect(getIP(), getPort());
				this.setConnected(connected);
			}
		};

		rcc.hookFeedbackProcessor(new IFeedbackProcessor() {

			@Override
			public void processFeedback(int nodeID, AxisData d, String status) {
				dashBoard.updatePanel(nodeID, d);
			
				if (Simulation.getInstance().isOpen()) {
					
					Simulation.getInstance().updateSim(nodeID + 1, d.q);
				}

			}

		});

		commandPanel = new CommandPanel(rcc) {
			// TODO: pickplane should be set in a better manner
			// not in command panel possibly
			private static final long serialVersionUID = 1L;

			@Override
			protected void setPickPlane(Object value) {
				tabbedPanel.setPickPlane(((Double) value).doubleValue());
			}
		};

		commandPanel.loadHistoryFromFile(new File(COMMAND_FILENAME));

		JButton launchSim = new JButton("Launch Simulator");
		launchSim.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				launchSimulatorFrame();
			}
		});

		LogConsolePanel console = new LogConsolePanel(theLogger);

		coreFrame.setLayout(new BoxLayout(coreFrame.getContentPane(),
				BoxLayout.Y_AXIS));

		coreFrame.add(cPanel);
		coreFrame.add(commandPanel);
		coreFrame.add(launchSim);
		coreFrame.add(tabbedPanel);
		coreFrame.add(dashBoard);
		coreFrame.add(console);

		coreFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		coreFrame.pack();
		coreFrame.addWindowListener(new WindowAdapter() {
			@Override
			public void windowClosing(WindowEvent e) {
				handleExit();
			}
		});

		coreFrame.setLocationRelativeTo(null);
		coreFrame.setTitle("SmallWorx by Robots Alive!");

		// enableComponents(false);

		GamePadManager gpm = new GamePadManager(coreFrame) {
			@Override
			public void axisChanged(String axisName, double axisValue) {
				tabbedPanel.axisChanged(axisName, axisValue);
			}

			@Override
			public void buttonChanged(int buttonID, int state) {
				tabbedPanel.buttonChanged(buttonID, state);
			}

			@Override
			public void directionChanged(double x, double y) {
				tabbedPanel.directionChanged(x, y);
			}
		};
		gpm.startPad();
	}

	void enableComponents(boolean b) {
		UIManager.enableComponent(tabbedPanel, b);
		UIManager.enableComponent(commandPanel, b);
	}

	protected void launchSimulatorFrame() {
		Simulation.launch();
	}

	protected boolean connect(String ip, int port) {
		 if (rcc.isConnected()) {
			disconnect();
			return false;
		}

		if (rcc.connect(ip, port, ip, port + 1)) {
			rcc.startFeedbackLoop();
			return true;
		}

		return false;
	}

	private void disconnect() {
		rcc.disconnect();
	}
}
