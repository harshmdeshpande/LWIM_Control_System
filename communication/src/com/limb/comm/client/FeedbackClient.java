package com.limb.comm.client;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.StringTokenizer;

public class FeedbackClient {

	private Socket feedbackSock;
	private PrintWriter out;
	private BufferedReader in;
	// private DatagramPacket packet;
	// private InetAddress address;

	private IFeedbackProcessor fbProcessor;

	public FeedbackClient() {
	}

	public void hookFeedbackProcessor(IFeedbackProcessor fp) {
		this.fbProcessor = fp;
	}

	public boolean connect(String IP, int feedbackPort) {
		try {
			System.out.println(IP + " " + feedbackPort);
			feedbackSock = new Socket(IP, feedbackPort);
			feedbackSock.setTcpNoDelay(true);
			out = new PrintWriter(feedbackSock.getOutputStream(), true);
			in = new BufferedReader(new InputStreamReader(feedbackSock
					.getInputStream()));
			// address = InetAddress.getByName(IP);
			// feedbackSock.joinGroup(address);

			// byte message[] = new byte[256];
			// packet = new DatagramPacket(message, message.length);
			return true;
		} catch (UnknownHostException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}

		return false;
	}

	// public boolean connectMultiCast(String IP, int feedbackPort) {
	// try {
	// feedbackSock = new MulticastSocket(feedbackPort);
	// address = InetAddress.getByName(IP);
	// feedbackSock.joinGroup(address);
	//
	// byte message[] = new byte[256];
	// packet = new DatagramPacket(message, message.length);
	// return true;
	// } catch (UnknownHostException e) {
	// e.printStackTrace();
	// } catch (IOException e) {
	// e.printStackTrace();
	// }
	//
	// return false;
	// }

	public boolean disconnect() {
		try {
			// if (address != null)

			feedbackSock.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		feedbackSock = null;
		return true;
	}

	public void startFeedbackLoop() {
		Thread t = new Thread(feedbackThread);
		t.start();
	}

	private final Runnable feedbackThread = new Runnable() {
		@Override
		public void run() {
			while (feedbackSock != null) {
				communicate();
				// try {
				// Thread.sleep(1);
				// } catch (InterruptedException e) {
				// e.printStackTrace();
				// }
			}
		}

		public void communicate() {
			try {
				if (fbProcessor != null) {
					String resultString = getResult();
					// System.out.println(resultString);
					StringTokenizer st = new StringTokenizer(resultString, "#");
					String cartesian = st.nextToken();
					int node = 0;
					while (st.hasMoreTokens()) {
						AxisData data = processAxisData(st.nextToken());
						fbProcessor.processFeedback(node, data, cartesian);
						node++;
					}
				}
			} catch (Exception e) {
			}
		}

		private String getResult() throws IOException {
			out.println("a");
			return in.readLine(); // new String(packet.getData(), 0,
									// packet.getLength());
		}
	};

	public boolean isConnected() {
		if (feedbackSock == null)
			return false;

		return true;
	}

	protected AxisData processAxisData(String axisData) {
		AxisData data = new AxisData();
		StringTokenizer axisT = new StringTokenizer(axisData);
		data.status = getBoolean(axisT.nextToken());
		data.ticks = (int) getDouble(axisT.nextToken());
		data.q = getDouble(axisT.nextToken());
		data.qp = getDouble(axisT.nextToken());
		data.isCalib = getBoolean(axisT.nextToken());
		return data;
	}

	private boolean getBoolean(String doubleString) {
		int x = ((int) (1000.0 * Double.parseDouble(doubleString))) / 1000;
		if (x > 0) {
			return true;
		}
		return false;
	}

	private double getDouble(String doubleString) {
		int x = (int) (1000 * Double.parseDouble(doubleString));
		return (x * 1.0 / 1000.0);
	}

	public static void main(String[] args) {
		FeedbackClient fbc = new FeedbackClient();
		fbc.connect("localhost", 2001);
		fbc.hookFeedbackProcessor(new IFeedbackProcessor() {

			@Override
			public void processFeedback(int nodeID, AxisData d, String status) {
				System.out.println(nodeID + " " + d.ticks);
			}

		});
		fbc.startFeedbackLoop();
	}
}
