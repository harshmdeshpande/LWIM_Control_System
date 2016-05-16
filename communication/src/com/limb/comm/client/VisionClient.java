package com.limb.comm.client;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

public class VisionClient {

	private Socket commandSock;
	private PrintWriter commandWriter;
	private BufferedReader responseReader;

	public VisionClient() {
	}

	public boolean connect(String IP, int port) {
		try {
			commandSock = new Socket(IP, port);
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

	public String sendMessage(String message) throws IOException {
		System.out.println("sending...");
		commandWriter.println(message);
		System.out.println("sent msg " + message);
		return responseReader.readLine();
	}

	public boolean isConnected() {
		if (commandSock == null)
			return false;
		return commandSock.isConnected();
	}

	public static void main(String[] args) {
		VisionClient client = new VisionClient();
		try {
			if (client.connect("172.16.10.192", 2000)) {
				while (client.isConnected()) {
//					client.commandWriter.println();
//					System.out.println(client.responseReader.readLine());
					System.out.println("received " + client.sendMessage("a"));
				}
				System.out.println("out");
				client.disconnect();
			}
		} catch (Exception e) {
			// TODO: handle exception
		}

	}
}
