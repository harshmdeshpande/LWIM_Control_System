package com.limb.ui.panels;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;

import javax.swing.AbstractAction;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JTextPane;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyleContext;

import org.apache.log4j.AppenderSkeleton;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.apache.log4j.spi.LoggingEvent;

public class LogConsolePanel extends JPanel {

	private static final long serialVersionUID = 1L;

	private JTextPane textArea = new JTextPane();

	public LogConsolePanel(Logger theLogger) {

		this.setLayout(new GridLayout());
		try {
			// Add a scrolling text area
			textArea.setEditable(false);
			textArea.setFocusable(true);
			textArea.setPreferredSize(new Dimension(100, 120));
			textArea.setFont(new Font(Font.SANS_SERIF, Font.BOLD, 12));
			this.add(new JScrollPane(textArea), BorderLayout.CENTER);

			JPopupMenu pop = new JPopupMenu();

			pop.add(new AbstractAction("Clear Console") {

				private static final long serialVersionUID = 1L;

				@Override
				public void actionPerformed(ActionEvent e) {
					clearConsole();
				}
			});
			textArea.setComponentPopupMenu(pop);

			// WriterAppender a = new WriterAppender();
			// a.setLayout(new SimpleLayout());
			// a.setWriter(new PrintWriter(System.out));

			theLogger.addAppender(new TextAppender(textArea));
		} catch (Exception e) {

		}
	}

	protected void clearConsole() {
		textArea.setText("");
	}

	class TextAppender extends AppenderSkeleton {

		private JTextPane ui;

		public TextAppender(JTextPane textArea) {
			this.ui = textArea;
		}

		@Override
		protected void append(LoggingEvent log) {
			String logMessage = log.getMessage().toString() + "\n";
			Level logLevel = log.getLevel();

			Document doc = ui.getDocument();

			StyleContext sc = StyleContext.getDefaultStyleContext();
			
			Color c = Color.BLUE;
			if (logLevel.equals(Level.ERROR)) {
				c = Color.RED;
			}

			if (logLevel.equals(Level.WARN)) {
				c = Color.DARK_GRAY;
			}
			
		    AttributeSet aset = sc.addAttribute(SimpleAttributeSet.EMPTY,
		        StyleConstants.Foreground, c);

			try {
				doc.insertString( doc.getLength(), logMessage, aset);
				ui.setCaretPosition(doc.getLength());
			} catch (BadLocationException e) {
				e.printStackTrace();
			}
		}

		@Override
		public void close() {
		}

		@Override
		public boolean requiresLayout() {
			return false;
		}

	}

	public static void main(String[] args) {
		JFrame f = new JFrame();
		try {
			Logger logger = Logger.getLogger(JFrame.class);
			LogConsolePanel c = new LogConsolePanel(logger);
			f.add(c);
			f.pack();
			f.setVisible(true);
			while (true) {
				logger.error("Hello");
				Thread.sleep(30);
			}
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}
}
