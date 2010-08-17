/*
 * Style.java
 *
 * Created on July 2, 2007, 11:07 PM
 */

package eug.shared;

import java.io.BufferedWriter;
import java.io.IOException;

/**
 * Collection of preferences determining how a <code>GenericObject</code> tree
 * will appear when printed out.
 * <p>
 * There are also four implementations: {@link #DEFAULT}, {@link #AGCEEP},
 * {@link #NO_WHITESPACE}, and {@link #EU3_SAVE_GAME}.
 * @author Michael Myers
 * @see WritableObject#toFileString(BufferedWriter, int, Style)
 * @see GenericObject#toFileString(BufferedWriter, Style)
 * @see GenericObject#toFileString(BufferedWriter, String, Style)
 */
public interface Style {
    
    public String getTab(int depth);
    public String getEqualsSign(int depth);
    public String getCommentStart();
    public void printTab(final BufferedWriter bw, int depth) throws IOException;
    public void printEqualsSign(final BufferedWriter bw, int depth) throws IOException;
    public void printCommentStart(final BufferedWriter bw, int depth) throws IOException;
    public void printHeaderCommentStart(final BufferedWriter bw, int depth) throws IOException;
    public void printHeaderCommentEnd(final BufferedWriter bw, int depth) throws IOException;
    public boolean isInline(final GenericObject obj);
    public boolean newLineAfterObject();
    
    
    // Implementations
    
    /**
     * <code>Style</code> that prints the same way as pre-EU3 save games. Tabs
     * consist of four spaces, and objects with three or fewer
     * children are printed on a single line (with a few exceptions).
     * Header comments are printed as in most EU2 event files: with a line of
     * hashes before and after. There is a space between the hash mark and the
     * comment text.
     */
    //<editor-fold defaultstate="collapsed" desc=" DEFAULT ">
    public static final Style DEFAULT = new Style() {
        
        public String getTab(int depth) {
            final StringBuilder sb = new StringBuilder(depth*4);
            for (int i = 0; i < depth; i++)
                sb.append("    ");
            return sb.toString();
        }
        
        public String getEqualsSign(int depth) {
            return " = ";
        }
        
        public String getCommentStart() {
            return "# ";
        }
        
        public void printTab(final BufferedWriter bw, int depth) throws IOException {
            for (int i = 0; i < depth; i++)
                bw.write("    ");
        }
        
        public void printEqualsSign(final BufferedWriter bw, int depth) throws IOException {
            bw.write(" = ");
        }
        
        public void printHeaderCommentStart(final BufferedWriter bw, int depth) throws IOException {
            printTab(bw, depth);
            
            for (int i = depth*GenericObject.tabLength; i < 80; i++)
                bw.write('#');
            
            bw.newLine();
        }
        
        public void printCommentStart(final BufferedWriter bw, int depth) throws IOException {
            bw.write("# ");
        }
        
        public void printHeaderCommentEnd(final BufferedWriter bw, int depth) throws IOException {
            bw.newLine();
            
            printTab(bw, depth);
            
            for (int i = depth*GenericObject.tabLength; i < 80; i++)
                bw.write('#');
        }
        
        public boolean isInline(final GenericObject obj) {
            if (obj == null)
                return false;
            
            final String name = obj.name;
            final int size = obj.size();
            
            if (name.equals("root"))
                return false;
            
            boolean sameLine = (size == 0 ||
                    (
                    (size < 4) &&
                    !(
                    name.equalsIgnoreCase("trigger") || name.startsWith("action_") ||
                    (size != 1 && name.equalsIgnoreCase("NOT") || (name.equalsIgnoreCase("AND") || name.equalsIgnoreCase("OR")))
                    )
                    ));
            if (sameLine) {
                for (ObjectVariable v : obj.values)
                    if (v.getInlineComment().length() != 0)
                        return false;
                for (GenericObject o : obj.children)
                    if (o.getInlineComment().length() != 0)
                        return false;
                for (GenericList l : obj.lists)
                    if (l.getInlineComment().length() != 0)
                        return false;
            }
            return sameLine;
        }
        
        public boolean newLineAfterObject() {
            return true;
        }
    };
    //</editor-fold>
    
    /**
     * <code>Style</code> that formats the same way as AGCEEP event files. Tabs
     * consist of a single tab character, and objects are printed on a single
     * line exactly when {@link #DEFAULT} would. Header comments have nothing
     * before or after. Comment text follows the hash mark without a space.
     */
    //<editor-fold defaultstate="collapsed" desc=" AGCEEP ">
    public static final Style AGCEEP = new Style() {
        
        public String getTab(int depth) {
            final StringBuilder sb = new StringBuilder(depth);
            for (int i = 0; i < depth; i++)
                sb.append("\t");
            return sb.toString();
        }
        
        public String getEqualsSign(int depth) {
            return " = ";
        }
        
        public String getCommentStart() {
            return "#";
        }
        
        public void printTab(final BufferedWriter bw, int depth) throws IOException {
            for (int i = 0; i < depth; i++)
                bw.write("\t");
        }
        
        public void printEqualsSign(final BufferedWriter bw, int depth) throws IOException {
            bw.write(" = ");
        }
        
        public void printHeaderCommentStart(final BufferedWriter bw, int depth) throws IOException {
            // do nothing
        }
        
        public void printCommentStart(final BufferedWriter bw, int depth) throws IOException {
            bw.write('#');
        }
        
        public void printHeaderCommentEnd(final BufferedWriter bw, int depth) throws IOException {
            // do nothing
        }
        
        public boolean isInline(final GenericObject obj) {
            // Same as default, I think.
            if (obj == null)
                return false;
            
            final String name = obj.name;
            final int size = obj.size();
            
            if (name.equals("root"))
                return false;
            
            boolean sameLine = (size == 0);
            if (sameLine) {
                    sameLine = (size < 4) && !(
                    name.equalsIgnoreCase("trigger") || name.startsWith("action_") ||
                    (size != 1 && (name.equalsIgnoreCase("NOT") || (name.equalsIgnoreCase("AND") || name.equalsIgnoreCase("OR"))))
                    );
            }
            if (sameLine) {
                for (ObjectVariable v : obj.values)
                    if (v.getInlineComment().length() != 0)
                        return false;
                for (GenericObject o : obj.children)
                    if (o.getInlineComment().length() != 0)
                        return false;
                for (GenericList l : obj.lists)
                    if (l.getInlineComment().length() != 0)
                        return false;
            }
            return sameLine;
        }
        
        public boolean newLineAfterObject() {
            return true;
        }
    };
    //</editor-fold>
    
    /**
     * <code>Style</code> that compresses as much as possible. There are no line
     * breaks unless there is a comment, and there are no spaces except between
     * variables. Comments are treated as in {@link #AGCEEP}.
     */
    //<editor-fold defaultstate="collapsed" desc=" NO_WHITESPACE ">
    public static final Style NO_WHITESPACE = new Style() {
        
        public String getTab(int depth) {
            return "";
        }
        
        public String getEqualsSign(int depth) {
            return "=";
        }
        
        public String getCommentStart() {
            return "#";
        }
        
        public void printTab(final BufferedWriter bw, int depth) throws IOException {
            // do nothing
        }
        
        public void printEqualsSign(final BufferedWriter bw, int depth) throws IOException {
            bw.write('=');
        }
        
        public void printHeaderCommentStart(final BufferedWriter bw, int depth) throws IOException {
            // do nothing
        }
        
        public void printCommentStart(final BufferedWriter bw, int depth) throws IOException {
            bw.write('#');
        }
        
        public void printHeaderCommentEnd(final BufferedWriter bw, int depth) throws IOException {
            // do nothing
        }
        
        public boolean isInline(final GenericObject obj) {
            if (obj == null)
                return false;
            
            final String name = obj.name;
            final int size = obj.size();
            
            if (name.equals("root"))
                return false;
            
            boolean sameLine = (size < 4);
            if (sameLine) {
                for (ObjectVariable v : obj.values)
                    if (v.getInlineComment().length() != 0)
                        return false;
                for (GenericObject o : obj.children)
                    if (o.getInlineComment().length() != 0)
                        return false;
                for (GenericList l : obj.lists)
                    if (l.getInlineComment().length() != 0)
                        return false;
            }
            return sameLine;
        }
        
        public boolean newLineAfterObject() {
            return false;
        }
    };
    //</editor-fold>
    
    /**
     * <code>Style</code> that formats the same way as EU3 save game files. Tabs
     * consist of four spaces, and objects are never inline. There is no space
     * on either side of equals signs. Comments are treated as in
     * {@link #AGCEEP}, for lack of a better reference.
     */
    //<editor-fold defaultstate="collapsed" desc=" EU3_SAVE_GAME ">
    public static final Style EU3_SAVE_GAME = new Style() {
        
        public String getTab(int depth) {
            final StringBuilder sb = new StringBuilder(depth*4);
            for (int i = 0; i < depth; i++)
                sb.append("    ");
            return sb.toString();
        }
        
        public String getEqualsSign(int depth) {
            return "=";
        }
        
        public String getCommentStart() {
            return "#";
        }
        
        public void printTab(final BufferedWriter bw, int depth) throws IOException {
            for (int i = 0; i < depth; i++)
                bw.write("    ");
        }
        
        public void printEqualsSign(final BufferedWriter bw, int depth) throws IOException {
            bw.write('=');
        }
        
        public void printHeaderCommentStart(final BufferedWriter bw, int depth) throws IOException {
            // do nothing
        }
        
        public void printCommentStart(final BufferedWriter bw, int depth) throws IOException {
            bw.write('#');
        }
        
        public void printHeaderCommentEnd(final BufferedWriter bw, int depth) throws IOException {
            // do nothing
        }
        
        public boolean isInline(final GenericObject obj) {
            return false;
        }
        
        public boolean newLineAfterObject() {
            return false;
        }
    };
    //</editor-fold>
}
