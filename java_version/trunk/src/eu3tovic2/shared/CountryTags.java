package eu3tovic2.shared;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;

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
		
		ArrayList<String> source = new ArrayList<String>();
		HashSet<String> destSet = new HashSet<String>(); //for error checking
		
		for (GenericObject link : root.getChild("mappings").children) {
			if (!link.name.equals("link")) {
				throw new IOException("Node which was not a link called " + link.name);
			}
			
			if (!link.contains("eu3") || !link.contains("vic")) {
				continue;
			}
			
			source.clear();
			String destTag = null;
			
			for (ObjectVariable value : link.values) {
				if (value.varname.equals("eu3")) {
					source.add(value.getValue());
				}
				else if (value.varname.equals("vic")) {
					if (destTag == null) {
						destTag = value.getValue();
						
						if (destSet.contains(destTag)) {
							System.err.println("Warning: Same destination country tag in multiple links: " +
									destTag);
						}
						else {
							destSet.add(destTag);
						}
					}
					else {
						System.err.println("Warning: Multiple destination country tags in same link: "
								+ destTag + " is prioritized instead of " + value.getValue());
					}
				}
				else {
					throw new IOException("Bad value in country link");
				}
			}
			
			for (String sourceTag : source) {
				if (ids.containsKey(sourceTag)) {
					System.err.println("Warning: Same source country tags in multiple links: " +
							sourceTag);
				}
				else {
					ids.put(sourceTag, destTag);
				}
			}
		}
	}
	
	public String convertTag(String tag) {
		String convTag = ids.get(tag);
		//if tag not found default to eu3 tag
		if (convTag == null) {
			return tag;
		}
		
		return convTag;
	}
	
	//singleton stuff
	static CountryTags instance;
	static public CountryTags getInstance() {
		if (instance == null) {
			try {
				instance = new CountryTags(new File("data/country_mappings.txt"));
			}
			catch (IOException e) {
				System.err.println(e.getMessage());
				System.exit(1);
			}
		}
		return instance;
	}
}
