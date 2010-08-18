package eu3tovic2.vic2model;

import java.util.Date;
import java.util.HashMap;

public class World {
	final Date date = new Date();
	String playerTag;
	HashMap<Integer, Province> provinces = new HashMap<Integer, Province>();
	HashMap<String, Country> countries = new HashMap<String, Country>();
	
	public Province[] getProvinces() {
		return provinces.values().toArray(new Province[1]);
	}
	
	public Date getDate() {
		return date;
	}

	public void setPlayer(String tag) {
		playerTag = tag;
	}

	public String getPlayer() {
		return playerTag;
	}
}
