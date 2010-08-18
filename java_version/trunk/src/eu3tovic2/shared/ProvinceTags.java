package eu3tovic2.shared;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;

import eug.parser.EUGFileIO;
import eug.shared.GenericList;
import eug.shared.GenericObject;
import eug.shared.ObjectVariable;

public class ProvinceTags {
	final private HashMap<Integer, ArrayList<Integer>> ids = new HashMap<Integer, ArrayList<Integer>>();
	
	private ProvinceTags(File file) throws IOException {
		GenericObject root = EUGFileIO.load(file);
		if (root == null) {
			throw new IOException("Failure to load province tag file " + file.getName());
		}
		
		ArrayList<Integer> source = new ArrayList<Integer>();
		
		for (GenericObject link : root.children) {
			if (!link.contains("eu3") || !link.contains("vic") ||
					!link.name.equals("link")) {
				System.out.println(link.name);
				System.out.println(link.contains("eu3"));
				System.out.println(link.contains("vic"));
				continue;
			}
			
			source.clear();
			ArrayList<Integer> dest = new ArrayList<Integer>();
			for (ObjectVariable value : link.values) {
				if (value.varname.equals("eu3")) {
					source.add(Integer.parseInt(value.getValue()));
				}
				else if (value.varname.equals("vic")) {
					dest.add(Integer.parseInt(value.getValue()));
				}
				else {
					throw new IOException("Bad value type in link");
				}
			}
			
			if (source.size() > 1) {
				System.err.println("Warning: Multiple source IDs in same link, all but first will be disregarded:");
				for (Integer id : source) {
					System.err.println(id);
				}
			}

			ids.put(source.get(0), dest);
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
