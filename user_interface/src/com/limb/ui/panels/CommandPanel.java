package com.limb.ui.panels;

import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSpinner;
import javax.swing.SpinnerNumberModel;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import com.limb.comm.client.IConnectionClient;

public class CommandPanel extends JPanel {

	private static final long serialVersionUID = 1L;
	private JComboBox commander;
	private IConnectionClient client;
	
	public CommandPanel(final IConnectionClient client) {
		this.client = client;

		this.setLayout(new FlowLayout(FlowLayout.LEADING));
		this.setBorder(BorderFactory.createEtchedBorder());

		JLabel label = new JLabel("Enter Command String: ", JLabel.CENTER);
		this.add(label);

		commander = new JComboBox();
		commander.setEditable(true);

		// TODO: see if we can implement an execute when ENTER is pressed!

		// .addKeyListener(new KeyAdapter() {
		// @Override
		// public void keyTyped(KeyEvent e) {
		// System.out.println(e);
		// if(e.getKeyCode() == KeyEvent.VK_ENTER) {
		// performCommand();
		// }
		// }
		// });

		this.add(commander);

		final JButton performCommandButton = new JButton("Send");
		performCommandButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				performCommand();
			}
		});

		final JButton clearHistoryButton = new JButton("Clear");
		clearHistoryButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				clearCommandHistory();
			}
		});

		final JButton stopRobotButton = new JButton("Stop Robot");
		stopRobotButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				client.sendMessage("halt");
			}
		});

		SpinnerNumberModel spinnerModel = new SpinnerNumberModel(0.015, 0.009,
				0.050, 0.001);
		final JSpinner pickPlane = new JSpinner(spinnerModel);
		pickPlane.addChangeListener(new ChangeListener() {

			@Override
			public void stateChanged(ChangeEvent e) {
				setPickPlane(pickPlane.getValue());
			}

		});

		this.add(performCommandButton);
		this.add(clearHistoryButton);
		this.add(stopRobotButton);
		this.add(new JLabel("PickPlane: "));
		this.add(pickPlane);

		setPickPlane(pickPlane.getValue());
	}

	protected void setPickPlane(Object value) {
		
	}

	protected void performCommand() {
		Object item = commander.getSelectedItem();
		if (item != null) {
			String commandString = commander.getSelectedItem().toString();
			String response = client.sendMessage(commandString);
			System.out.println("Robot Says: " + response);

			// TODO: Do we store commands which return error?
			if (response != null
					&& !response.startsWith(IConnectionClient.ERROR)) {
				updateHistory(commandString);
			}
		} else {
			System.out.println("Hmm!");
		}
	}

	private void updateHistory(String commandString) {
		if (!commandIsListed(commandString)) {
			commander.insertItemAt(commandString, 0);
		}
	}

	private boolean commandIsListed(String commandString) {
		int items = commander.getItemCount();
		for (int i = 0; i < items; i++) {
			if (commandString.equalsIgnoreCase(commander.getItemAt(i)
					.toString())) {
				return true;
			}
		}
		return false;
	}

	protected void clearCommandHistory() {
		commander.removeAllItems();
	}

	public void loadHistoryFromFile(File file) {
		try {
			FileInputStream fis = new FileInputStream(file);
			BufferedReader r = new BufferedReader(new InputStreamReader(fis));

			String cmd;

			while ((cmd = r.readLine()) != null) {
				commander.addItem(cmd);
			}

		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	// TODO: Debate if we keep ALL the history or only the last session commands
	public void saveHistoryToFile(File file) {
		try {
			PrintWriter w = new PrintWriter(file);

			int items = commander.getItemCount();
			for (int i = 0; i < items; i++) {
				w.println(commander.getItemAt(i).toString());
			}

			w.flush();

		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
