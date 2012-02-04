package eu3tovic2.shared;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;

import eug.parser.EUGFileIO;
import eug.shared.GenericObject;
import eug.shared.ObjectVariable;

public class ProvinceTags {
	final private HashMap<Integer, ArrayList<Integer>> ids = new HashMap<Integer, ArrayList<Integer>>();
	
	private ProvinceTags(File file) throws IOException {
		GenericObject root = EUGFileIO.load(file);
		if (root == null) {
			throw new IOException("Failure to load province tag file " + file.getName());
		}
		
		HashSet<Integer> destSet = new HashSet<Integer>(); //for error checking
		
		for (GenericObject link : root.getChild("mappings").children) {
			if (!link.name.equals("link")) {
				throw new IOException("Node which was not a link called " + link.name);
			}
			
			if (!link.contains("eu3") || !link.contains("vic")) {
				continue;
			}
			
			Integer sourceTag = null;
			ArrayList<Integer> dest = new ArrayList<Integer>();
			
			for (ObjectVariable value : link.values) {
				if (value.varname.equals("eu3")) {
					if (sourceTag == null) {
						sourceTag = Integer.parseInt(value.getValue());
					}
					else {
						System.err.println("Warning: Multiple source province IDs in same link: " +
								sourceTag + " is prioritized instead of " + value.getValue());
					}
				}
				else if (value.varname.equals("vic")) {
					Integer destTag = Integer.parseInt(value.getValue());
					
					if (destSet.contains(destTag)) {
						System.err.println("Warning: Same destination province ID in multiple links: " +
								destTag);
					}
					else {
						dest.add(destTag);
						destSet.add(destTag);
					}
				}
				else {
					throw new IOException("Bad value in province link");
				}
			}

			if (sourceTag != null) {
				if (ids.containsKey(sourceTag)) {
					System.err.println("Warning: Same source province ID in multiple links: " +
							sourceTag);
				}
				else {
					ids.put(sourceTag, dest);
				}
			}
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
				instance = new ProvinceTags(new File("data/province_mappings.txt"));
			}
			catch (IOException e) {
				System.err.println(e.getMessage());
				System.exit(1);
			}
		}
		return instance;
	}
}
