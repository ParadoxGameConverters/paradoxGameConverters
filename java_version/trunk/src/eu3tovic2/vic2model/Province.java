package eu3tovic2.vic2model;

public class Province {
	final int tag;
	
	public Province(World world, int tag) {
		this.tag = tag;
		
		if (world.provinces.containsKey(tag)) {
			throw new ModelException("Province with ID " + tag + " already exists");
		}
		
		world.provinces.put(tag, this);
	}

	public int getTag() {
		return tag;
	}
}
