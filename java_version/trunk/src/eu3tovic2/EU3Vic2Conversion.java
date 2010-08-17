package eu3tovic2;

import java.io.File;
import java.util.ArrayList;

import eug.shared.GenericObject;

import savefileconverter.ConversionStatus;

public class EU3Vic2Conversion implements Runnable {
	private final File file;
	private final ConversionStatus status;
	private int errorCounter;
	private final ArrayList<Runnable> tasks = new ArrayList<Runnable>();
	private GenericObject root;

	public EU3Vic2Conversion(File file, ConversionStatus status) {
		this.file = file;
		this.status = status;
		
		tasks.add(new LoadEu3(this));
	}
	
	public File getFile() {
		return file;
	}
	
	public void nextError(String line) {
		errorCounter++;
		status.printInfo(line);
	}
	
	public void nextInfo(String line) {
		status.printInfo(line);
	}
	
	@Override
	public void run() {
		try {
			for (int i = 0, n = tasks.size(); i < n; ++i) {
				Runnable t = tasks.get(i);
				status.conversionProgress(t.toString(), i, n);
				t.run();
				Thread.sleep(0);
			}
			
			status.conversionProgress("Finished", tasks.size(), tasks.size());
		}
		catch (InterruptedException e) {
			status.printError("Conversion was interrupted");
			errorCounter++;
		}
		
		status.conversionEnded(errorCounter == 0);
	}

	public void setRoot(GenericObject root) {
		this.root = root;
	}
}
