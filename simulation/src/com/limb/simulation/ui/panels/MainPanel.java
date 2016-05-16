package com.limb.simulation.ui.panels;

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;

import javax.robotics.engine.robots.Robot3D;
import javax.swing.JPanel;

import com.limb.simulation.control.KinematicsEngine;

public class MainPanel extends JPanel
{
    private static final long serialVersionUID = 3256442499685233201L;
    
	private KinematicsEngine kineEngine;
//	private ServerPanel serverPanel;
//	
//	private static final String CONNECTED = "Disconnect";
//	private static final String DISCONNECTED = "Connect";

    public MainPanel(Robot3D robot) {
    	
		createPanel();
		
//		client = new FeedbackClient();
//		client.hookFeedbackProcessor(new IFeedbackProcessor(){
//			
//			@Override
//			public String[] getRequests() {
//				return new String[]{"status", "pos"};
//			}
//
//			@Override
//			public void processFeedback(String[][] resultSet) {
//				kineEngine.simulateSolution(resultSet, true);
//			}
//		});
	}
    
    private void createPanel() {
    	
    	final GridBagLayout panelLayout = new GridBagLayout();
        GridBagConstraints c = new GridBagConstraints();
        c.fill = GridBagConstraints.BOTH;
        setLayout(panelLayout);
        
        c.gridx = 0;    
        
        c.gridy = 0;
        
        c.gridy = 1;
        
//		serverPanel = new ServerPanel("Connection") {
//			private static final long serialVersionUID = 1L;
//
//			@Override
//			protected void handleConn() {
//				handleConnection();
//			}
//			
//			@Override
//			protected void handleReset() {
//				doReset();
//			}
//		};
//		add(serverPanel, c);
        
        c.gridy = 2;
    }

    protected void doReset() {
		// if (client.isConnected()) {
		// //client.sendMessage("gohome");
		// // client.sendMessage("moveline 0.275 0 0.3 3.14 0 0 " +
		// serverPanel.getDuration());
		//    	}
	}

//	protected void handleConnection() {
//    	if (!client.isConnected()) {
//    		if (client.connect("localhost", 2001)) {
//    			client.startFeedbackLoop();
//    			serverPanel.setState(CONNECTED);
//    		}
//    	} else {
//    		client.disconnect();
//    		serverPanel.setState(DISCONNECTED);
//    	}
//    }

	public KinematicsEngine getKineEngine(){
		return kineEngine; 
	 }
    
}
