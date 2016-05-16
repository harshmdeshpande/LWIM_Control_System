package com.limb.ui.panels.helpers;

import java.awt.Component;

import javax.swing.JFrame;
import javax.swing.JOptionPane;

public class PathSelectionFrame extends JFrame {

	private static final long serialVersionUID = 1L;

	String[] pathID = {"Path_1", "Path_2", "Path_3"};
	
	public PathSelectionFrame() {
		super("Select Path");
	}

	private void createAndShowGUI() {

		

		this.setVisible(true);
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.pack();
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

	public static int okcancel(Component parent, String theMessage) {
		int result = JOptionPane.showConfirmDialog(parent,
				theMessage, "alert", JOptionPane.OK_CANCEL_OPTION);
		return result;
	}

	public static void main(String[] args) {
		 new PathSelectionFrame().start();

	}
}

/*
 * import java.awt.BorderLayout;
 * 
 * import javax.swing.JFrame; import javax.swing.JList; import
 * javax.swing.JScrollPane; import javax.swing.ListSelectionModel;
 * 
 * public class DragTest14 extends JFrame {
 * 
 * JList jl;
 * 
 * String[] items = { "Java", "C", "C++", "Lisp", "Perl", "Python" };
 * 
 * public DragTest14() { super("Drag Test 1.4"); setSize(200, 150);
 * setDefaultCloseOperation(EXIT_ON_CLOSE);
 * 
 * jl = new JList(items);
 * jl.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
 * jl.setDragEnabled(true);
 * 
 * getContentPane().add(new JScrollPane(jl), BorderLayout.CENTER);
 * setVisible(true); }
 * 
 * public static void main(String args[]) { new DragTest14(); } }
 */