package eu3tovic2.vic2model;

public class Country {
	final String tag;
	
	public Country(World world, String tag) {
		this.tag = tag;
		
		if (world.countries.containsKey(tag)) {
			throw new ModelException("Country with tag " + tag + " already exists");
		}
		
		world.countries.put(tag, this);
	}
}
