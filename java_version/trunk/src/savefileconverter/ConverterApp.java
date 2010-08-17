package savefileconverter;

import javax.swing.SwingUtilities;

public class ConverterApp {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		//for now, only show GUI version
		
		SwingUtilities.invokeLater(new Runnable() {
			@Override
			public void run() {
				new MainWindow().setVisible(true);
			}
		});
		
	}
}
