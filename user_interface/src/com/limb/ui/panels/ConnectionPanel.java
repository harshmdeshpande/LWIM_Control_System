package com.limb.ui.panels;

import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;

import com.limb.ui.PanelDemo;

public class ConnectionPanel extends JPanel {

	private static final long serialVersionUID = 1L;

	private JComboBox ipComboBox;
	private JComboBox portComboBox;
	private JButton connectButton;

	public ConnectionPanel(String IP, int port) {
		this.setLayout(new GridLayout(1, 5));
		addComponents(IP, port);
	}

	private void addComponents(String ip, int port) {
		JLabel ipLabel = new JLabel("IP Adress", JLabel.CENTER);
		ipComboBox = new JComboBox(new String[] { ip });
		JLabel portLabel = new JLabel("Port", JLabel.CENTER);
		portComboBox = new JComboBox(new Integer[] { port });

		connectButton = new JButton();
		connectButton.setText("Connect");
		connectButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				handleConnectionButtonClicked();
			}
		});
		this.add(ipLabel);
		this.add(ipComboBox);
		this.add(portLabel);
		this.add(portComboBox);
		this.add(connectButton);

	}
	
	protected void setConnected(boolean b) {
		PanelDemo.theLogger.info("Robot is " + (b ? "connected" : "not connected"));
		connectButton.setText(b ? "Disconnect" : "Connect");
	}

	protected void handleConnectionButtonClicked() {
		
	}

	public String getIP() {
		return (String) ipComboBox.getSelectedItem();
	}

	public int getPort() {
		return (Integer) portComboBox.getSelectedItem();
	}
}
