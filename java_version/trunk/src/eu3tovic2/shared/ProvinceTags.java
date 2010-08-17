package eu3tovic2.shared;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;

import eug.parser.EUGFileIO;
import eug.shared.GenericList;
import eug.shared.GenericObject;

public class ProvinceTags {
	final private HashMap<Integer, ArrayList<Integer>> ids = new HashMap<Integer, ArrayList<Integer>>();
	
	private ProvinceTags(File file) throws IOException {
		GenericObject root = EUGFileIO.load(file);
		if (root == null) {
			throw new IOException("Failure to load province tag file " + file.getName());
		}
		
		for (GenericList list : root.lists) {
			if (list.size() == 0) {
				continue;
			}
			
			Integer sourceId = Integer.parseInt(list.getName());
			ArrayList<Integer> destIds = new ArrayList<Integer>();
			for (String listVal : list) {
				destIds.add(Integer.parseInt(listVal));
			}
			
			ids.put(sourceId, destIds);
		}
	}
	
	public ArrayList<Integer> convertTag(Integer tag) {
		return ids.get(tag);
	}
	
	//singleton stuff
	static ProvinceTags instance;
	static public ProvinceTags getInstance() {
		if (instance == null) {
			try {
				instance = new ProvinceTags(new File("data/ProvinceTags.txt"));
			}
			catch (IOException e) {
				System.err.println(e.getMessage());
				System.exit(1);
			}
		}
		return instance;
	}
}
