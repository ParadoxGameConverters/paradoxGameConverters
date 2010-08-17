/*
 * Utilities.java
 *
 * Created on July 17, 2006, 1:14 PM
 */

package eug.shared;

import java.io.File;
import java.io.FileFilter;
import java.io.FilenameFilter;
import java.io.Serializable;
import java.text.DecimalFormat;
import java.util.Arrays;
import java.util.Comparator;

/**
 *
 * @author Michael Myers
 */
public final class Utilities {
    public static final DecimalFormat popFormatter = new DecimalFormat("##,###,##0.000");
    private static final FileDateComparator SORT_BY_DATE = new FileDateComparator();
    public static final EugFileFilter EUG_FILTER = new EugFileFilter();
    
    private Utilities() { }
    
    public static String formatPop(double pop) {
        return popFormatter.format(pop);
    }
    
    public static String findNewestFile(File dir) {
        File[] list = dir.listFiles(/*EUG_FILTER*/);
        Arrays.sort(list, SORT_BY_DATE);
        return list[0].getAbsolutePath();
    }
    
    public static String findNewestFile(File dir, FileFilter filter) {
        File[] list = dir.listFiles(filter);
        Arrays.sort(list, SORT_BY_DATE);
        return list[0].getAbsolutePath();
    }
    
    public static String findNewestFile(File dir, FilenameFilter filter) {
        File[] list = dir.listFiles(filter);
        Arrays.sort(list, SORT_BY_DATE);
        return list[0].getAbsolutePath();
    }

    // Implement Serializable so that any list that uses this comparator can be
    // serialized.
    private static final class FileDateComparator implements Comparator<File>, Serializable {
        private static final long serialVersionUID = 1L;
        public int compare(File f1, File f2) {
            long f1mod = f1.lastModified();
            long f2mod = f2.lastModified();
            return (f1mod < f2mod ? 1 : (f1mod == f2mod ? 0 : -1));
        }
    }

    private static final class EugFileFilter implements FileFilter {
        public boolean accept(File pathname) {
            return pathname.getName().endsWith(".eug") || pathname.getName().endsWith(".eu3");
        }
    }


}
