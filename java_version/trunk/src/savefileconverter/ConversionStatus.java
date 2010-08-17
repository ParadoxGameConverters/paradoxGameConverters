package savefileconverter;

public interface ConversionStatus {
	public void conversionEnded(boolean success);
	public void printInfo(String line);
	public void printError(String line);
	public void conversionProgress(String name, int task, int numTasks);
	public void taskProgress(float fraction);
}
