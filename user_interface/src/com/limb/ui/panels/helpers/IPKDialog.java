package com.limb.ui.panels.helpers;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.DefaultListModel;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSpinner;
import javax.swing.SpinnerNumberModel;

import com.limb.comm.client.IConnectionClient;
import com.limb.ui.panels.TabbedPanel;

public class IPKDialog extends JDialog {

	private static final long serialVersionUID = 1L;

	private TabbedPanel panel;

	private List<JSpinner> coordinates = new ArrayList<JSpinner>();

	private CartesianPanel cartesianPanel = new CartesianPanel();
	private SolutionPanel solutionPanel = new SolutionPanel();

	public IPKDialog(TabbedPanel panel) {
		this.panel = panel;

		this.setLayout(new BoxLayout(this.getContentPane(), BoxLayout.Y_AXIS));

		this.getContentPane().add(cartesianPanel, BorderLayout.CENTER);
		this.getContentPane().add(solutionPanel, BorderLayout.CENTER);
		this.pack();
		
		this.setAlwaysOnTop(true);
		this.setModal(true);
		this.setLocationRelativeTo(panel.getFrame());
		this.setTitle("Inverse Position Kinematics");
	}

	private class CartesianPanel extends JPanel {

		private static final long serialVersionUID = 1L;

		public CartesianPanel() {
			this.setLayout(new GridLayout(1, 3));
			this.setBorder(BorderFactory
					.createTitledBorder("Cartesian Coordinates"));

			JPanel positionPanel = new JPanel();
			positionPanel.setLayout(new BoxLayout(positionPanel,
					BoxLayout.Y_AXIS));
			positionPanel
					.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

			JPanel x = createCartesianElement("x : ", 1.0);
			JPanel y = createCartesianElement("y : ", 1.0);
			JPanel z = createCartesianElement("z : ", 1.0);

			positionPanel.add(x);
			positionPanel.add(y);
			positionPanel.add(z);

			JPanel orientationPanel = new JPanel();
			orientationPanel.setLayout(new BoxLayout(orientationPanel,
					BoxLayout.Y_AXIS));
			orientationPanel.setBorder(BorderFactory.createEmptyBorder(5, 5, 5,
					5));
			JPanel rx = createCartesianElement("rx : ", 2 * Math.PI);
			JPanel ry = createCartesianElement("ry : ", 2 * Math.PI);
			JPanel rz = createCartesianElement("rz : ", 2 * Math.PI);

			orientationPanel.add(rx);
			orientationPanel.add(ry);
			orientationPanel.add(rz);

			JButton solveButton = new JButton("Solve");
			solveButton.addActionListener(new ActionListener() {

				@Override
				public void actionPerformed(ActionEvent arg0) {
					solutionPanel.clearAll();

					String[] solutionList = getSolutions();
					solutionPanel.updateSolutions(solutionList);
				}

			});

			this.add(positionPanel);
			this.add(orientationPanel);
			this.add(solveButton);
		}

		protected String[] getSolutions() {
			String coord = "solve ";
			for (int i = 0; i < 6; i++) {
				double x = roundTo((Double) coordinates.get(i).getModel()
						.getValue());
				coord += x + " ";
			}
			String msg = panel.sendMessage(coord);
			StringTokenizer st = new StringTokenizer(msg, "#");

			String[] solutions = new String[st.countTokens()];
			for (int i = 0; i < solutions.length; i++) {
				solutions[i] = st.nextToken();
			}
			return solutions;
		}

		private JPanel createCartesianElement(String name, double absLimit) {
			JPanel panel = new JPanel();
			panel.setLayout(new BoxLayout(panel, BoxLayout.X_AXIS));
			this.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

			JLabel label = new JLabel(name);
			JSpinner spinner = new JSpinner(new SpinnerNumberModel(0.0,
					-absLimit, absLimit, 0.001));

			coordinates.add(spinner);

			panel.add(label);
			panel.add(spinner);
			return panel;
		}

		public void initKine(double[] pos) {
			for (int i = 0; i < pos.length; i++) {
				coordinates.get(i).setValue(pos[i]);
			}
		}

	}

	private class SolutionPanel extends JPanel {

		private static final long serialVersionUID = 1L;

		private DefaultListModel solutionListModel;
		private JList solutionList;

		public SolutionPanel() {
			this.setLayout(new GridLayout());

			solutionListModel = new DefaultListModel();
			solutionList = new JList(solutionListModel);
			solutionList.addMouseListener(new MouseAdapter() {

				@Override
				public void mouseClicked(MouseEvent e) {
					if (e.getClickCount() == 2) {
						handleSelection();
					}
				}
			});

			this.add(new JScrollPane(solutionList));
		}

		protected void handleSelection() {
			String msg = (String) solutionList.getSelectedValue();
			if (!msg.startsWith(IConnectionClient.ERROR)) {
				System.out.println("Solution selected : " + msg);
				panel.sendMessage("setjpos " + msg);
			} else {
				System.out.println("Invalid Selection");
			}
		}

		public void clearAll() {
			solutionListModel.clear();
		}

		public void updateSolutions(String[] solutions) {
			for (int i = 0; i < solutions.length; i++) {
				solutionListModel.addElement(solutions[i]);
			}
		}
	}

	public void setVisible() {
		initKine();
		this.setVisible(true);
	}

	private void initKine() {
		String msg = panel.sendMessage("kine");
		StringTokenizer st = new StringTokenizer(msg, " ");

		double[] pos = new double[6];
		for (int i = 0; i < 6; i++) {
			pos[i] = Double.parseDouble(st.nextToken());
		}

		cartesianPanel.initKine(pos);
	}

	private double roundTo(double x) {
		return x = (x * 1000.0) / 1000.0;
	}

}
