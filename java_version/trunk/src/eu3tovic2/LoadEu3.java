package eu3tovic2;

import eug.parser.EUGFileIO;
import eug.parser.ParserSettings;
import eug.shared.GenericObject;

class LoadEu3 implements Runnable {
	private final EU3Vic2Conversion conv;

	public LoadEu3(EU3Vic2Conversion conv) {
		this.conv = conv;
	}
	
	@Override
	public void run() {
		ParserSettings settings = ParserSettings.getDefaults();
		settings.setTryToRecover(true);
		GenericObject root = EUGFileIO.load(conv.getFile());
		if (root == null) {
			conv.nextError("Unable to parse EUE save file");
		}
		
		conv.setRoot(root);
	}

	@Override
	public String toString() {
		return "Load EU3 save file into memory";
	}
}
