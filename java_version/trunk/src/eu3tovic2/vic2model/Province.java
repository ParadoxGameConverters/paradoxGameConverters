package eu3tovic2.vic2model;

import java.util.ArrayList;
import java.util.List;

public class Province {
	final int tag;
	String owner = "";
	String controller = "";
	final ArrayList<String> coreTo = new ArrayList<String>();
	
	public Province(World world, int tag) {
		this.tag = tag;
		
		if (world.provinces.containsKey(tag)) {
			throw new ModelException("Province with ID " + tag + " already exists");
		}
		
		world.provinces.put(tag, this);
	}
	
	public Province(World world, int tag, String owner, String controller, List<String> coreTo) {
		this(world, tag);
		
		this.owner = owner;
		this.controller = controller;
		this.coreTo.addAll(coreTo);
	}

	public int getTag() {
		return tag;
	}

	public String getOwner() {
		// TODO Auto-generated method stub
		return null;
	}
}
