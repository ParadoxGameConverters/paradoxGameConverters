package eu3tovic2.shared;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;

import eug.parser.EUGFileIO;
import eug.shared.GenericObject;
import eug.shared.ObjectVariable;

public class CountryTags {
	final private HashMap<String, String> ids = new HashMap<String, String>();
	
	private CountryTags(File file) throws IOException {
		GenericObject root = EUGFileIO.load(file);
		if (root == null) {
			throw new IOException("Failure to load country tag file " + file.getName());
		}
		
		for (ObjectVariable var : root.values) {
			ids.put(var.varname, var.getValue());
		}
	}
	
	public String convertTag(String tag) {
		return ids.get(tag);
	}
	
	//singleton stuff
	static CountryTags instance;
	static public CountryTags getInstance() {
		if (instance == null) {
			try {
				instance = new CountryTags(new File("data/CountryTags.txt"));
			}
			catch (IOException e) {
				System.err.println(e.getMessage());
				System.exit(1);
			}
		}
		return instance;
	}
}
