package eu3tovic2.vic2model;

import java.util.HashMap;

public class World {
	HashMap<Integer, Province> provinces = new HashMap<Integer, Province>();
	HashMap<String, Country> countries = new HashMap<String, Country>();
	
	public Province[] getProvinces() {
		return provinces.values().toArray(new Province[1]);
	}
}
