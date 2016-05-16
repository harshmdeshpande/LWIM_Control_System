package com.limb.ui.panels.helpers;

import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JProgressBar;

public class OperationDialog {

	private JDialog theDialog;
	private String title;

	public OperationDialog(JFrame parent, String title) {
		this.title = title;
		theDialog = new JDialog(parent, title, true);
		FlowLayout lay = new FlowLayout(FlowLayout.LEFT);
		theDialog.setLayout(lay);
				
		JProgressBar progressBar = new JProgressBar();
		progressBar.setIndeterminate(true);
	
		theDialog.setDefaultCloseOperation(JDialog.DO_NOTHING_ON_CLOSE);
		theDialog.setLocationRelativeTo(parent);
		
		JButton operationCancelButton = new JButton("Cancel Operation");
		operationCancelButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				theDialog.setTitle("Cancelling...");
				handleButtonClicked();
			}
		});
		
		
		theDialog.add(progressBar);
		theDialog.add(operationCancelButton);
		theDialog.pack();

		/*
		 * pickPlaceDialog.setAlwaysOnTop(true);
		 * pickPlaceDialog.setModalityType(Dialog.ModalityType.APPLICATION_MODAL);
		 * pickPlaceDialog.setDefaultCloseOperation(JDialog.DO_NOTHING_ON_CLOSE);
		 */

		// TODO: Add progress monitor
		// pickPlaceDialog.add()
		// JButton pickPlaceCloseButton = new JButton("Cancel Operation");
		// pickPlaceCloseButton.addActionListener(new ActionListener() {
		// @Override
		// public void actionPerformed(ActionEvent e) {
		// cancelPickPlaceOperation();
		// }
		// });
		// pickPlaceDialog.add(pickPlaceCloseButton);
		// pickPlaceDialog.pack();
	}

	public void show(boolean block) {
		theDialog.setTitle(title);
		if (!block) {
			new Thread() {
				@Override
				public void run() {
					theDialog.setVisible(true);
				}
			}.start();
		} else {
			theDialog.setVisible(true);
		}

	}

	public void hide() {
		theDialog.setVisible(false);
	}
	
	protected void handleButtonClicked() {
		
	}
	
	
	
	private static OperationDialog o;
	public static void main(String[] args) {
		o = new OperationDialog(null, "Operation") {
			@Override
			protected void handleButtonClicked() {
				try {
					Thread.sleep(1000);
					o.hide();
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		};
		o.show(false);
	}
}
