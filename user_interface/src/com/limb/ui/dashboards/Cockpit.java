package com.limb.ui.dashboards;

import java.awt.GridLayout;
import java.util.ArrayList;
import java.util.List;

import javax.swing.JPanel;

import com.limb.comm.client.AxisData;
import com.limb.ui.panels.AxisPanel;

public class Cockpit extends JPanel {

	private static final int NODES = 6;

	private static final long serialVersionUID = 1L;

	private List<AxisPanel> panelList;
	private AxisPanel activePanel = null;

	public Cockpit() {
		this.setLayout(new GridLayout(NODES, 1));
		panelList = new ArrayList<AxisPanel>();

		for (int i = 1; i <= NODES; i++) {
			AxisPanel aPanel = new AxisPanel("Node " + i);
			panelList.add(aPanel);
			this.add(aPanel);
		}
	}

	public void updatePanel(int nodeID, AxisData d) {
		panelList.get(nodeID).setAxisData(d);
	}

	public void highlightNode(int nodeID) {
		if (activePanel != null) {
			activePanel.highlight(false);
		}
		if (nodeID != 0) {
			activePanel = panelList.get(nodeID - 1);
			activePanel.highlight(true);
		}
	}
}
