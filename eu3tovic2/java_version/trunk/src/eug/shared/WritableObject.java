/*
 * Interface implemented by GenericObject, GenericList and ObjectVariable.
 *
 * Used to write them to a file.
 */

package eug.shared;

import java.io.BufferedWriter;
import java.io.IOException;

public interface WritableObject {
    
    public void toFileString(BufferedWriter bw, int depth) throws IOException;
    /** @since EUGFile 1.06.00pre1 */
    public void toFileString(BufferedWriter bw, int depth, Style style) throws IOException;
}
