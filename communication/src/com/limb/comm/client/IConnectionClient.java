package com.limb.comm.client;

public interface IConnectionClient {

	String ERROR = "ERROR";
	
	boolean isConnected();	
	String sendMessage(String command);
	

}
