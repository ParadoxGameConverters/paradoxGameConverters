package eu3tovic2.tasks;

import java.io.File;

import savefileconverter.ConversionStatus;
import eu3tovic2.ConversionState;
import eug.parser.EUGFileIO;

public class SaveVic2 implements Runnable {
	private final ConversionState state;
	@SuppressWarnings("unused")
	private final ConversionStatus status;

	public SaveVic2(ConversionState state, ConversionStatus status) {
		this.state = state;
		this.status = status;
		
		state.outputFile = new File(state.inputFile.getPath() + ".v2"); //TODO: set in conversion class later
	}
	
	@Override
	public void run() {
		EUGFileIO.save(state.outputRoot, state.outputFile.getPath(),
				"EU3ToVic2 converted save game", true);
	}

	@Override
	public String toString() {
		return "Save to Vic 2 save file";
	}
}
