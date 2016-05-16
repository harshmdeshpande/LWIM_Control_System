package com.limb.comm.client;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

public class CommandClient implements IConnectionClient {

	private Socket commandSock;
	private PrintWriter commandWriter;
	private BufferedReader responseReader;

	public CommandClient() {
	}

	public boolean connect(String IP, int port) {
		try {
			commandSock = new Socket(IP, port);
			commandSock.setTcpNoDelay(true);
			commandWriter = new PrintWriter(commandSock.getOutputStream(), true);
			responseReader = new BufferedReader(new InputStreamReader(
					commandSock.getInputStream()));
			return true;
		} catch (UnknownHostException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}

		return false;
	}

	public boolean disconnect() {
		try {
			commandSock.close();
			commandSock = null;
		} catch (IOException e) {
			e.printStackTrace();
		}
		return true;
	}

	public String sendMessage(String message) {
		// System.out.println(message);
		if (commandWriter != null) {
			commandWriter.println(message);
			try {
				return responseReader.readLine();
			} catch (IOException e) {
				e.printStackTrace();
				disconnect();
			}
		}
		return "ERROR check connection!";
	}

	public boolean isConnected() {
		if (commandSock == null)
			return false;
		return commandSock.isConnected();
	}

	public static void main(String[] args) {
		CommandClient client = new CommandClient();
		client.connect("localhost", 2000);

		client.sendMessage("movejdir 6 1");
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}

		client.sendMessage("halt");
	}
}
