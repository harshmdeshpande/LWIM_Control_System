package com.limb.ui.panels;

import java.awt.Component;
import java.awt.GridLayout;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;

import javax.swing.BorderFactory;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;

import com.limb.comm.client.IConnectionClient;
import com.limb.ui.PanelDemo;
import com.limb.ui.core.IJSController;
import com.limb.ui.core.modes.ModeManager;
import com.limb.ui.panels.tabs.ModeTab;

public class TabbedPanel extends JPanel {

	private static final long serialVersionUID = 1L;
	private JTabbedPane modePane;
	private double pickPlane = 0.018;

	private ModeTab calibrationTab;// = new ModeTab(new CalibrationMode(
	// new HomingAppManager()));
	private ModeTab programTab;// = new ModeTab(new ProgramMode());
	private ModeTab executionTab;// = new ModeTab(new ExecutionMode(
	// new DummyAppManager(), new InsertionManager(), new PNPManager()));

	private IConnectionClient client;
	private int activeAxis = 0;

	public double mx, nx, jx, my, ny, jy;

	private JFrame frame;

	public TabbedPanel(IConnectionClient client, JFrame parent) {
		this.client = client;
		this.frame = parent;
		this.setLayout(new GridLayout());
		this.setBorder(BorderFactory.createEtchedBorder());

		modePane = new JTabbedPane();
		this.add(modePane);
		this.setBorder(BorderFactory.createTitledBorder("Operation Mode"));

		ModeManager modeManager = new ModeManager();

		calibrationTab = new ModeTab(modeManager.getCalibrationMode());
		programTab = new ModeTab(modeManager.getProgramMode());
		executionTab = new ModeTab(modeManager.getExecutionMode());

		modePane.addTab(calibrationTab.getModeName(), calibrationTab);
		modePane.addTab(programTab.getModeName(), programTab);
		modePane.addTab(executionTab.getModeName(), executionTab);

		// modeManager.startStateMachine();

	}

	private void initializeMapParams() {
		File paramFile = new File("params.dat");
		try {
			FileInputStream fis = new FileInputStream(paramFile);
			BufferedReader r = new BufferedReader(new InputStreamReader(fis));

			this.mx = getDouble(r.readLine());
			this.nx = getDouble(r.readLine());
			this.jx = getDouble(r.readLine());

			this.my = getDouble(r.readLine());
			this.ny = getDouble(r.readLine());
			this.jy = getDouble(r.readLine());

		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	private double getDouble(String s) {
		return Double.parseDouble(s);
	}

	public void axisChanged(String axisName, double axisValue) {
		IJSController c = getActiveController();
		if (c != null) {
			c.handleAxisChanged(axisName, axisValue);
		}
	}

	public void buttonChanged(int buttonID, int state) {
		IJSController c = getActiveController();
		if (c != null) {
			c.handleButtonChanged(buttonID, state);
		}
	}

	public void directionChanged(double x, double y) {
		IJSController c = getActiveController();
		if (c != null) {
			c.handleDirectionChanged(x, y);
		}
	}

	private IJSController getActiveController() {
		Component c = modePane.getSelectedComponent();
		if (c instanceof ModeTab) {
			return ((ModeTab) c).getActiveController();
		}

		return null;
	}

	public int getActiveAxis() {
		return this.activeAxis;
	}

	public void setActiveAxis(int axis) {
		if (axis < 0)
			axis = 0;

		if (axis > 6)
			axis = 6;

		this.activeAxis = axis;
		selectAxis(axis);
	}

	public String sendMessage(String command) {
		if (client != null) {
			PanelDemo.theLogger.info("Message: " + command);
			return client.sendMessage(command);
		}

		return "ERROR: check connection!";
	}

	protected void selectAxis(int axis) {
		// to be overridden
	}

	public void handleDisconnection(boolean disconnected) {
		if (disconnected) {
			// homingTab.finishMode();
			// veloControlTab.finishMode();
		}
	}

	public IConnectionClient getCommClient() {
		return this.client;
	}

	public JFrame getFrame() {
		return frame;
	}

	public void saveParameters(double[] params) {
		try {
			File paramFile = new File("params.dat");
			if (!paramFile.exists()) {
				paramFile.createNewFile();
			}
			PrintWriter w = new PrintWriter(paramFile);
			for (int i = 0; i < params.length; i++) {
				w.println(params[i]);
			}
			w.flush();
		} catch (Exception e) {
			e.printStackTrace();
		}

		initializeMapParams();
	}

	public double getPickPlane() {
		return this.pickPlane;
	}

	public void setPickPlane(double value) {
		this.pickPlane = value;
	}

	// public static void main(String[] args) {
	// double[] x = {1, 2, 3, 4, 5, 6};
	// (new TabbedPanel(null, null)).saveParameters(x);
	// }
}
