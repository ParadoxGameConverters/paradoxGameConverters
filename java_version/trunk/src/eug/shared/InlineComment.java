/*
 * InlineComment.java
 *
 * Created on July 11, 2006, 10:22 PM
 */

package eug.shared;

import java.io.BufferedWriter;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Michael Myers
 */
public class InlineComment extends Comment {
    
//    /** @since EUGFile 1.02.00 */
//    public static boolean spaceBeforeComment = true;
    
    /** Creates a new instance of InlineComment */
    public InlineComment(String comment) {
        super(comment);
    }
    
    public void toFileString(BufferedWriter bw, int depth) throws IOException {
        toFileString(bw, depth, Style.DEFAULT);
    }
    
    public void toFileString(BufferedWriter bw, int depth, Style style) throws IOException {
        if (comment.length() == 0) {
            bw.newLine();
            return;
        }
        
        boolean firstLine = true;
        
        for (String str : newLinePattern.split(comment)) {
            if (firstLine)
                firstLine = false;
            else {
                bw.newLine();
                style.printTab(bw, depth);
            }
            
            style.printCommentStart(bw, depth);
            bw.write(str);
        }
//        bw.newLine();
    }
    
//    /** @since EUGFile 1.01.03 */
//    protected static final void writeCommentLine(final BufferedWriter bw, final String c, final Style s)
//    throws IOException {
//        bw.write(commentChar);
//        if (spaceBeforeComment)
//            bw.write(' ');
//        bw.write(c);
//    }
    
    public String toString() {
//        System.out.println("Called InlineComment.toString()");
        return toString(Style.DEFAULT);
    }
    
    public String toString(Style s) {
        return s.getCommentStart() + comment;
    }

    @Override
    public InlineComment clone() {
        try {
            return (InlineComment)super.clone();
        } catch (CloneNotSupportedException ex) {
            Logger.getLogger(InlineComment.class.getName()).log(Level.SEVERE, null, ex);
            return null;
        }
    }
}
