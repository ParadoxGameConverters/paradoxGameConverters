package eu3tovic2;

import java.io.File;

import eu3tovic2.vic2model.World;
import eug.shared.GenericObject;

public class ConversionState {
	public File inputFile;
	public File outputFile; //set by SaveVic2 task for now
	public int errorCount;
	public GenericObject inputRoot;
	public World worldModel = new World(); //TODO: perhaps separate task is needed
}
