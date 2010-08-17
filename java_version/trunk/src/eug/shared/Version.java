/*
 * Version.java
 *
 * Created on July 11, 2006, 9:13 PM
 */

package eug.shared;

import java.text.DecimalFormat;
import java.text.NumberFormat;

/**
 * This class holds the version information for the EUGFile library.
 * @author Michael Myers
 */
public final class Version {
    private static final NumberFormat minorVersionFormat = NumberFormat.getInstance();
    static {
        if (minorVersionFormat instanceof DecimalFormat) {
            ((DecimalFormat) minorVersionFormat).setMinimumIntegerDigits(2);
        }
    }
    
    /** No Version instances are allowed. */
    private Version() { }
    
    public static String getVersion() {
        return String.format("%s %d.%s.%s %s",
                getName(), getMajorVersion(),
                minorVersionFormat.format(getMinorVersion()),
                minorVersionFormat.format(getUpdate()), getState()//, getBuild()
                );
    }
    
    public static String getName() {
        return "EUGFile Library";
    }
    
    public static int getMajorVersion() {
        return 1;
    }
    
    public static int getMinorVersion() {
        return 6;
    }
    
    public static int getUpdate() {
        return 2;
    }
    
//    public static int getBuild() {
//        return 1;
//    }
    
    public static String getState() {
        return "";
    }
}
