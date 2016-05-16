package com.limb.simulation.ui.panels;

import java.awt.Color;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JTextField;

public class ServerPanel extends JPanel {

	private static final long serialVersionUID = 1L;

	private JTextField fDur;
	private JButton btn_connection;

	public ServerPanel(String title) {
		createPanel(title);
	}

	private void createPanel(String title) {
		setBorder(BorderFactory.createCompoundBorder(BorderFactory
				.createTitledBorder(BorderFactory.createLineBorder(Color.gray),
						title), BorderFactory.createEmptyBorder(2, 5, 2, 5)));

		final GridBagLayout panelLayout = new GridBagLayout();
		GridBagConstraints c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		setLayout(panelLayout);

		btn_connection = new JButton("Connect");
		JButton btn_reset = new JButton("Reset");

		add(btn_connection, c);
		add(btn_reset, c);

		btn_connection.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				handleConn();
			}
		});
		
		btn_reset.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				handleReset();
			}
		});
		
		fDur = new JTextField("5.0");
		add(fDur, c);
	}

	protected void handleReset() {
		
	}

	protected void handleConn() {		
	}
	
	double getDuration() {
		return Double.parseDouble(fDur.getText());
	}

	public void setState(String text) {
		btn_connection.setText(text);
	}
}
