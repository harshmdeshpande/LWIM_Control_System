package com.limb.ui.panels.helpers;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.DefaultListModel;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;

import com.limb.ui.panels.TabbedPanel;

public class DualListBox extends JDialog {

	private static final long serialVersionUID = 1L;

	private static final Insets EMPTY_INSETS = new Insets(0, 0, 0, 0);

	private static final String ADD_BUTTON_LABEL = "Add >>";
	private static final String REMOVE_BUTTON_LABEL = "<< Remove";
	private static final String DEFAULT_SOURCE_CHOICE_LABEL = "Available Paths";
	private static final String DEFAULT_DEST_CHOICE_LABEL = "Selected Paths";

	private JLabel sourceLabel;
	private JList sourceList;
	private DefaultListModel sourceListModel;
	private JList destList;
	private DefaultListModel destListModel;
	private JLabel destLabel;
	private JButton addButton;
	private JButton removeButton;

	public DualListBox(TabbedPanel panel, String title) {
		this.setLayout(new BoxLayout(this.getContentPane(), BoxLayout.Y_AXIS));
		initScreen();
		this.setAlwaysOnTop(true);
		this.setModal(true);
		this.setLocationRelativeTo(panel.getFrame());
		this.setTitle(title);
	}

	public void addSourceElements(Object newValue[]) {
		fillListModel(sourceListModel, newValue);
	}

	public void addDestinationElements(Object newValue[]) {
		fillListModel(destListModel, newValue);
	}

	private void fillListModel(DefaultListModel model, Object newValues[]) {
		for (int i = 0; i < newValues.length; i++) {
			model.addElement(newValues[i]);
		}
	}

	private void clearSourceSelected(Object[] selected) {
		for (int i = selected.length - 1; i >= 0; --i) {
			sourceListModel.removeElement(selected[i]);
		}
		sourceList.getSelectionModel().clearSelection();
	}

	private void clearDestinationSelected(Object[] selected) {
		for (int i = selected.length - 1; i >= 0; --i) {
			destListModel.removeElement(selected[i]);
		}
		destList.getSelectionModel().clearSelection();
	}

	private void initScreen() {
		JPanel panel = new JPanel();
		panel.setBorder(BorderFactory.createEtchedBorder());
		panel.setLayout(new GridBagLayout());

		sourceLabel = new JLabel(DEFAULT_SOURCE_CHOICE_LABEL);
		sourceListModel = new DefaultListModel();// new SortedListModel();
		sourceList = new JList(sourceListModel);
		panel.add(sourceLabel, new GridBagConstraints(0, 0, 1, 1, 0, 0,
				GridBagConstraints.CENTER, GridBagConstraints.NONE,
				EMPTY_INSETS, 0, 0));

		panel.add(new JScrollPane(sourceList), new GridBagConstraints(0, 1, 1,
				5, .5, 1, GridBagConstraints.CENTER, GridBagConstraints.BOTH,
				EMPTY_INSETS, 0, 0));

		addButton = new JButton(ADD_BUTTON_LABEL);
		panel.add(addButton, new GridBagConstraints(1, 2, 1, 2, 0, .25,
				GridBagConstraints.CENTER, GridBagConstraints.NONE,
				EMPTY_INSETS, 0, 0));
		addButton.addActionListener(new AddListener());
		removeButton = new JButton(REMOVE_BUTTON_LABEL);
		panel.add(removeButton, new GridBagConstraints(1, 4, 1, 2, 0, .25,
				GridBagConstraints.CENTER, GridBagConstraints.NONE, new Insets(
						0, 5, 0, 5), 0, 0));
		removeButton.addActionListener(new RemoveListener());

		destLabel = new JLabel(DEFAULT_DEST_CHOICE_LABEL);
		destListModel = new DefaultListModel();// SortedListModel();
		destList = new JList(destListModel);
		panel.add(destLabel, new GridBagConstraints(2, 0, 1, 1, 0, 0,
				GridBagConstraints.CENTER, GridBagConstraints.NONE,
				EMPTY_INSETS, 0, 0));
		panel.add(new JScrollPane(destList), new GridBagConstraints(2, 1, 1, 5,
				.5, 1.0, GridBagConstraints.CENTER, GridBagConstraints.BOTH,
				EMPTY_INSETS, 0, 0));

		JPanel setCancelPanel = new JPanel();
		setCancelPanel.setLayout(new FlowLayout(FlowLayout.TRAILING));
		JButton setButton = new JButton("Execute");
		setButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				handlePathSelection();
			}
		});

		JButton cancelButton = new JButton("Cancel");
		cancelButton.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {
				// System.out.println("path cancelled");
				handlePathCancelled();
			}
		});
		setCancelPanel.add(setButton);
		setCancelPanel.add(cancelButton);

		this.getContentPane().add(panel, BorderLayout.CENTER);
		this.getContentPane().add(setCancelPanel, BorderLayout.CENTER);
		this.pack();
	}

	protected void handlePathCancelled() {
	}

	protected void handlePathSelection() {
	}

	public static void main(String args[]) {

		new DualListBox(null, "Test").setVisible(true);

	}

	private class AddListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			Object selected[] = sourceList.getSelectedValues();
			addDestinationElements(selected);
			clearSourceSelected(selected);
		}
	}

	private class RemoveListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			Object selected[] = destList.getSelectedValues();
			addSourceElements(selected);
			clearDestinationSelected(selected);
		}
	}

	public int[] getSelectedPaths() {
		int size = destListModel.getSize();
		int[] idList = new int[size];

		if (size > 0) {
			for (int i = 0; i < size; i++) {
				idList[i] = Integer.parseInt((String) destListModel.get(i));
				System.out.println(idList[i]);
			}
		}

		return idList;
	}

	public void clearDestination() {
		destListModel.removeAllElements();
	}

	public void clearSource() {
		sourceListModel.removeAllElements();
	}

	@Override
	public void setEnabled(boolean b) {
		super.setEnabled(b);

	}
}
