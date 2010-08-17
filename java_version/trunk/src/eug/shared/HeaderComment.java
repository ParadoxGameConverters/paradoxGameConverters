/*
 * HeaderComment.java
 *
 * Created on July 11, 2006, 10:20 PM
 */

package eug.shared;

import java.io.BufferedWriter;
import java.io.IOException;

/**
 *
 * @author Michael Myers
 */
public class HeaderComment extends InlineComment {
    
    /** Creates a new instance of HeaderComment */
    public HeaderComment(String comment) {
        super(comment);
    }
    
//    /**
//     * @deprecated As of EUGFile 1.01.03, use
//     * {@link Comment#appendComment appendComment} instead.
//     */
//    @Deprecated
//    public void addCommentLine(String newComment) {
//        super.appendComment(newComment);
////        comment += "\n" + newComment;
//    }
    
    public void toFileString(BufferedWriter bw, int depth) throws IOException {
        toFileString(bw, depth, Style.DEFAULT);
    }
    
    public void toFileString(BufferedWriter bw, int depth, Style style) throws IOException {
        if (comment.length() == 0)
            return;
        
//        writeHashLine(bw, depth);
        style.printHeaderCommentStart(bw, depth);
        final String[] lines = newLinePattern.split(comment);
        final int num = lines.length;
        for (int i = 0; i < num; i++) {
            GenericObject.writeTab(bw, depth);
            style.printCommentStart(bw, depth);
            bw.write(lines[i]);
            if (i < num-1)
                bw.newLine();
        }
        style.printHeaderCommentEnd(bw, depth);
//        writeHashLine(bw, depth);
        bw.newLine();
    }
    
//    private static final void writeHashLine(BufferedWriter bw, int depth) throws IOException {
//        GenericObject.writeTab(bw, depth);
//
//        for (int i = depth*GenericObject.tabLength; i < 80; i++)
//            bw.write(commentChar);
//
//        bw.newLine();
//    }
    
}
