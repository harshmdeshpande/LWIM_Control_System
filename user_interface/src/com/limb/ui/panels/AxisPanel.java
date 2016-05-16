package com.limb.ui.panels;

import java.awt.Color;
import java.awt.GridLayout;

import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JPanel;

import com.limb.comm.client.AxisData;

public class AxisPanel extends JPanel {

	private JLabel axisPositionField;
	private JLabel axisQ;
	private JLabel axisVelocityField;
	private JLabel axisStatusField;
	private JLabel axisCalibField;

	private static final long serialVersionUID = 1L;
	private String axisID;
	private JLabel axisNameLabel;

	public AxisPanel(String axisID) {
		this.axisID = axisID;
		initPanel();
	}

	private void initPanel() {
		this.setLayout(new GridLayout(1, 7));
		this.setBorder(BorderFactory.createEtchedBorder());

		initComponents(this);
	}

	private void initComponents(JPanel panel) {
		axisNameLabel = createLabel("Axis " + axisID, JLabel.LEADING);
		axisNameLabel.setForeground(Color.BLUE);
		
		JLabel axisStatusLabel = createLabel("Status ", JLabel.TRAILING);
		axisStatusField = new JLabel("Off");
		//axisStatusField.setEnabled(false);

		JLabel axisPositionLabel = createLabel("Ticks ", JLabel.TRAILING);
		axisPositionField = new JLabel("0");
		// axisPositionField.setBorder(BorderFactory.createLoweredBevelBorder());

		JLabel axisHomeLabel = createLabel("Q ", JLabel.TRAILING);
		axisQ = new JLabel("0");
		// axisHomeField.setBorder(BorderFactory.createLoweredBevelBorder());

		JLabel axisVelocityLabel = createLabel("Qdot ", JLabel.TRAILING);
		axisVelocityField = new JLabel("0");
		// axisVelocityField.setBorder(BorderFactory.createLoweredBevelBorder());

		JLabel axisCalibLabel = createLabel("Calibrated ", JLabel.TRAILING);
		axisCalibField = new JLabel("No");
		//axisCalibField.setEnabled(false);

		panel.add(axisNameLabel);
		panel.add(axisStatusLabel);
		panel.add(axisStatusField);
		panel.add(axisPositionLabel);
		panel.add(axisPositionField);
		panel.add(axisHomeLabel);
		panel.add(axisQ);
		panel.add(axisVelocityLabel);
		panel.add(axisVelocityField);
		panel.add(axisCalibLabel);
		panel.add(axisCalibField);
	}

	private JLabel createLabel(String txt, int style) {
		JLabel lbl = new JLabel(txt, style);
		lbl.setEnabled(false);
		return lbl;
	}
	
	public long getVelo() {
		String velo = axisVelocityField.getText();
		return Long.parseLong(velo);
	}

	public void setAxisData(AxisData d) {
		//status can be 0 or 1;
		boolean stat = d.status;
		axisStatusField.setForeground(stat ? Color.GREEN : Color.RED);
		axisStatusField.setText(stat ? "OK" : "Disabled");
	
		axisPositionField.setText("" + d.ticks);
		
		axisQ.setText("" + d.q + " [" + degrees(d.q)  + "]");
		
		axisVelocityField.setText("" + d.qp);
		
		stat = d.isCalib;
		axisCalibField.setForeground(stat ? Color.GREEN : Color.RED);
		axisCalibField.setText(stat ? "YES" : "NO");	
	}

	private double degrees(double radians) {
		int x = (int) (100 * Math.toDegrees(radians));
		return x/100;
	}

	public void highlight(boolean b) {
		axisNameLabel.setEnabled(b);
	}
}
