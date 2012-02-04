package eu3tovic2;

import java.io.File;
import java.util.ArrayList;

import eu3tovic2.tasks.*;
import savefileconverter.ConversionStatus;

public class EU3Vic2Conversion implements Runnable {
	private final ConversionState state = new ConversionState();
	private final ConversionStatus status;
	private final ArrayList<Runnable> tasks = new ArrayList<Runnable>();
	

	public EU3Vic2Conversion(File file, ConversionStatus status) {
		this.state.inputFile = file;
		this.status = status;
		
		tasks.add(new LoadEu3(state, status));
		tasks.add(new BuildVic2AST(state, status));
		tasks.add(new SaveVic2(state, status));
	}
	
	@Override
	public void run() {
		try {
			for (int i = 0, n = tasks.size(); i < n; ++i) {
				Runnable t = tasks.get(i);
				status.conversionProgress(t.toString(), i, n);
				t.run();
				
				Thread.sleep(0); //checks for thread interrupts
				if (state.errorCount > 0) {
					break;
				}
			}
			
			status.conversionProgress("Finished", tasks.size(), tasks.size());
		}
		catch (InterruptedException e) {
			status.printError("Conversion was interrupted");
			state.errorCount++;
		}
		
		status.conversionEnded(state.errorCount == 0);
	}
}
