package eu3tovic2.tasks;

import savefileconverter.ConversionStatus;
import eu3tovic2.ConversionState;
import eug.parser.EUGFileIO;
import eug.parser.ParserSettings;

public class LoadEu3 implements Runnable {
	private ConversionState state;
	private ConversionStatus status;

	public LoadEu3(ConversionState state, ConversionStatus status) {
		this.state = state;
		this.status = status;
	}
	
	@Override
	public void run() {
		ParserSettings settings = ParserSettings.getDefaults();
		settings.setTryToRecover(true);
		settings.setPrintTimingInfo(false);
		state.inputRoot = EUGFileIO.load(state.inputFile);
		
		if (state.inputRoot == null) {
			status.printError("Unable to parse EU3 save file");
			state.errorCount++;
		}
	}

	@Override
	public String toString() {
		return "Load EU3 save file into memory";
	}
}
