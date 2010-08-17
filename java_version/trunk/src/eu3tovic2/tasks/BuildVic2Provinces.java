package eu3tovic2.tasks;

import java.io.IOException;
import java.util.ArrayList;

import savefileconverter.ConversionStatus;
import eu3tovic2.ConversionState;
import eu3tovic2.shared.ProvinceTags;
import eu3tovic2.vic2model.Province;
import eug.shared.GenericObject;

public class BuildVic2Provinces implements Runnable {
	private final ConversionState state;
	private final ConversionStatus status;

	public BuildVic2Provinces(ConversionState state, ConversionStatus status) {
		this.state = state;
		this.status = status;
	}
	
	@Override
	public void run() {
		for (GenericObject child : state.inputRoot.children) {
			//check first character of node name to determine if it's province id
			if (!Character.isDigit(child.name.charAt(0))) {
				continue;
			}
			
			Integer sourceId = Integer.parseInt(child.name);
			ArrayList<Integer> destIds = ProvinceTags.getInstance().convertTag(sourceId);
			if (destIds == null) {
				continue;
			}

			for (Integer id : destIds) {
				new Province(state.worldModel, id);
			}
		}
	}
	
	public String toString() {
		return "Build Vic 2 provinces";
	}
}
