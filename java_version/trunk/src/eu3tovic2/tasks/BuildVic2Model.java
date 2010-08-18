package eu3tovic2.tasks;

import java.util.ArrayList;
import java.util.Date;
import java.util.Scanner;

import savefileconverter.ConversionStatus;
import eu3tovic2.ConversionState;
import eu3tovic2.shared.CountryTags;
import eu3tovic2.shared.ProvinceTags;
import eu3tovic2.vic2model.Province;
import eug.shared.GenericObject;
import eug.shared.ObjectVariable;

public class BuildVic2Model implements Runnable {
	private final ConversionState state;
	@SuppressWarnings("unused")
	private final ConversionStatus status;

	public BuildVic2Model(ConversionState state, ConversionStatus status) {
		this.state = state;
		this.status = status;
	}
	
	@Override
	public void run() {
		for (ObjectVariable var : state.inputRoot.values) {
			if (var.varname.equals("date")) {
				Date date = buildDate(var.getValue());
				state.worldModel.getDate().setTime(date.getTime());
			}
			else if (var.varname.equals("player")) {
				state.worldModel.setPlayer(CountryTags.getInstance().convertTag(var.getValue()));
			}
		}
		
		for (GenericObject child : state.inputRoot.children) {
			//check first character of node name to determine if it's province id
			if (Character.isDigit(child.name.charAt(0))) {
				buildProvince(child);
			}
		}
	}
	
	private void buildProvince(GenericObject provinceNode) {
		Integer sourceId = Integer.parseInt(provinceNode.name);
		ArrayList<Integer> destIds = ProvinceTags.getInstance().convertTag(sourceId);
		if (destIds == null) {
			return;
		}
		
		String owner = CountryTags.getInstance().convertTag(provinceNode.getString("owner"));
		String controller = CountryTags.getInstance().convertTag(provinceNode.getString("controller"));
		ArrayList<String> coreTo = new ArrayList<String>();
		for (String core : provinceNode.getStrings("core")) {
			coreTo.add(CountryTags.getInstance().convertTag(core));
		}

		for (Integer id : destIds) {
			new Province(state.worldModel, id, owner, controller, coreTo);
		}
	}

	@SuppressWarnings("deprecation")
	private static Date buildDate(String value) { //TODO: build an EU date class as replacement, this is too advanced for its own good
		Scanner scan = new Scanner(value);
		scan.useDelimiter("\\.");
		int y = scan.nextInt();
		int m = scan.nextInt();
		int d = scan.nextInt();
		
		Date date = new Date();
		date.setYear(y);
		date.setMonth(m);
		date.setDate(d);
		date.setHours(12);
		date.setMinutes(0);
		date.setSeconds(0);
		
		return date;
	}
	
	public String toString() {
		return "Build Vic 2 world model";
	}
}
