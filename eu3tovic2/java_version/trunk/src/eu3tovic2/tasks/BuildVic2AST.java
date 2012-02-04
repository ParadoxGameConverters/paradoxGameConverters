package eu3tovic2.tasks;

import java.util.ArrayList;
import java.util.HashMap;
import savefileconverter.ConversionStatus;
import eu3tovic2.ConversionState;
import eu3tovic2.shared.CountryTags;
import eu3tovic2.shared.ProvinceTags;
import eug.shared.GenericObject;

public class BuildVic2AST implements Runnable {
	private final ConversionState state;
	@SuppressWarnings("unused")
	private final ConversionStatus status;
	
	private final HashMap<String, GenericObject> builtCountries =
		new HashMap<String, GenericObject>();

	public BuildVic2AST(ConversionState state, ConversionStatus status) {
		this.state = state;
		this.status = status;
	}
	
	@Override
	public void run() {
		state.outputRoot = new GenericObject();
		
		state.outputRoot.addString("date", state.inputRoot.getString("date"));
		state.outputRoot.addString("start_date", state.inputRoot.getString("date"));
		String convertedPlayer = CountryTags.getInstance().convertTag(state.inputRoot.getString("player"));
		state.outputRoot.addString("player", convertedPlayer);
		state.outputRoot.addBlankLine();
		
		for (GenericObject child : state.inputRoot.children) {
			//check first character of node name to determine if it's province id
			if (Character.isDigit(child.name.charAt(0))) {
				buildProvince(child);
			}
			//interpret as country tag if upper case and three letters
			else if (child.name.length() == 3 &&
					Character.isUpperCase(child.name.charAt(0)) &&
					Character.isUpperCase(child.name.charAt(1)) &&
					Character.isUpperCase(child.name.charAt(2))) {
				buildCountry(child);
			}
		}
	}
	
	private void buildProvince(GenericObject source) {
		Integer sourceId = Integer.parseInt(source.name);
		ArrayList<Integer> destIds = ProvinceTags.getInstance().convertTag(sourceId);
		if (destIds == null) {
			return;
		}
		
		GenericObject template = new GenericObject();
		
		String convertedOwner = CountryTags.getInstance().convertTag(source.getString("owner"));
		if (convertedOwner != "") {
			template.addString("owner", convertedOwner);
		}
		
		String convertedController = CountryTags.getInstance().convertTag(source.getString("controller"));
		if (convertedController != "") {
			template.addString("controller", convertedController);
		}
		
		for (String core : source.getStrings("core")) {
			String convertedCore = CountryTags.getInstance().convertTag(core);
			template.addString("core", convertedCore);
		}

		//generated new provinces, for now simply copy template
		for (Integer id : destIds) {
			GenericObject province = template.deepClone();
			province.name = String.valueOf(id);
			province.setHeadComment("From EU3 province " + source.name);
			state.outputRoot.addChild(province);
			state.outputRoot.addBlankLine();
		}
	}
	
	private void buildCountry(GenericObject source) {
		String convertedTag = CountryTags.getInstance().convertTag(source.name);
		
		if (builtCountries.containsKey(convertedTag)) {
			//for now do nothing more
			return;
		}
		
		GenericObject country = new GenericObject();
		country.name = convertedTag;
		
		if (source.getString("human").equals("yes")) {
			country.addString("human", "yes");
		}
		
		ArrayList<Integer> convertedCapital = ProvinceTags.getInstance().
			convertTag(Integer.parseInt(source.getString("capital")));
		if (convertedCapital == null) {
			return;
		}
		
		country.addString("capital", String.valueOf(convertedCapital.get(0)));
		
		country.setHeadComment("From EU3 country " + source.name);
		state.outputRoot.addChild(country);
		state.outputRoot.addBlankLine();
		
		builtCountries.put(convertedTag, country);
	}
	
	public String toString() {
		return "Build Vic 2 world model";
	}
}
