package savefileconverter;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JEditorPane;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.JScrollPane;

import eu3tovic2.EU3Vic2Conversion;

class MainWindow extends JFrame implements ConversionStatus {
	private static final long serialVersionUID = -8926420164595294212L;
	
	private JFileChooser conversionChooser;
	private JEditorPane logArea;
	private JProgressBar conversionProgress;
	private JProgressBar taskProgress;
	private JButton startButton;
	private JButton cancelButton;
	
	private Thread conversionThread;
	private Object conversionLock = new Object();
	
	public MainWindow() {
		super("EU2ToVic3 Converter");
		
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setResizable(false);
		setLayout(new BorderLayout());
		setLocationByPlatform(true);
		initComponents();
		pack();
	}
	
	private void initComponents() {
		conversionChooser = new JFileChooser("./test");
		
		JPanel northPanel = new JPanel();
		northPanel.setLayout(new BoxLayout(northPanel, BoxLayout.X_AXIS));
		add(northPanel, BorderLayout.NORTH);
		
		startButton = new JButton("Convert...");
		startButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				lookForConversion();
			}
		});
		northPanel.add(startButton);
		
		cancelButton = new JButton("Exit");
		cancelButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				cancel();
			}
		});
		northPanel.add(cancelButton);

		logArea = new JEditorPane();
		logArea.setBorder(BorderFactory.createTitledBorder("Conversion log:"));
		logArea.setEditable(false);
		Dimension dim = new Dimension(400, 200);
		logArea.setMinimumSize(dim);
		logArea.setPreferredSize(dim);
		
		JScrollPane logAreaScroll = new JScrollPane(logArea);
		logAreaScroll.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		add(logAreaScroll, BorderLayout.CENTER);
		
		JPanel southPanel = new JPanel();
		southPanel.setBorder(BorderFactory.createTitledBorder("Progress (overall/per task):"));
		southPanel.setLayout(new GridLayout(2, 1));
		add(southPanel, BorderLayout.SOUTH);
		
		conversionProgress = new JProgressBar();
		southPanel.add(conversionProgress);
		
		taskProgress = new JProgressBar();
		southPanel.add(taskProgress);
	}
	
	private void lookForConversion() {
		int result = conversionChooser.showOpenDialog(this);
		if (result != JFileChooser.APPROVE_OPTION)  {
			return;
		}
		
		beginConversion(conversionChooser.getSelectedFile());
	}
	
	private void cancel() {
		synchronized (conversionLock) {
			if (conversionThread == null) {
				System.exit(0);
			}
			else {
				conversionThread.interrupt();
			}
		}
	}
	
	private void beginConversion(File file) {
		startButton.setEnabled(false);
		cancelButton.setText("Cancel");
		logArea.setText("");
		
		printInfo("Starting conversion of " + file.getName());
		
		conversionThread = new Thread(new EU3Vic2Conversion(file, this));
		conversionThread.setPriority(Thread.MIN_PRIORITY);
		conversionThread.start(); //threadsafe as this method is called by startButton which is disabled if thread is running
	}
	
	@Override
	public void conversionEnded(boolean success) {
		synchronized (conversionLock) {
			conversionThread = null;
			startButton.setEnabled(true);
			cancelButton.setText("Exit");
			
			printInfo("Conversion finished " + (success? "successfully" : "unsuccessfully"));
		}
	}
	
	@Override
	public void conversionProgress(String name, int task, int numTasks) {
		printInfo("Beginning conversion task: " + name);
		
		conversionProgress.setMinimum(0);
		conversionProgress.setMaximum(numTasks);
		conversionProgress.setValue(task);
	}
	
	@Override
	public void printError(String line) {
		printInfo(line); //no color coding for now
	}
	
	@Override
	public void printInfo(String line) {
		//inefficient method but shouldn't matter since log is cleared between conversions
		logArea.setText(logArea.getText() + line + '\n');
	}
	
	@Override
	public void taskProgress(float fraction) {
		// TODO Auto-generated method stub
		
	}
}
