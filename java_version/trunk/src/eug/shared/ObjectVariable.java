
package eug.shared;

import java.io.BufferedWriter;
import java.io.IOException;

/**
 * Representes an attribute of a GenericObject.
 */
public final class ObjectVariable
        implements WritableObject, Cloneable, Comparable<ObjectVariable> {
    public String varname;
    private String value;
    //is the value stored in quotes or not? (religion = catholic vs culture = "french")
    public boolean inQuotes;
    
    private InlineComment headComment;
    private InlineComment inlineComment;
    
    ObjectVariable(String name, String val) {
        this(name, val, false);
    }
    
    ObjectVariable(String name, String val, boolean quotes) {
        varname = name;
        value = val;
        inQuotes = quotes;
    }
    
    public void setValue(String newValue) {
        value = newValue;
        inQuotes = false;
    }
    
    public void setValue(String newValue, boolean quotes) {
        value = newValue;
        inQuotes = (quotes || newValue.contains(" "));
    }
    
    public String getValue() {
        return value;
    }
    
    public void setHeadComment(String comment) {
        if (comment == null || comment.length() == 0)
            this.headComment = null;
        else
            this.headComment = new InlineComment(comment);
    }
    
    public String getHeadComment() {
        return (headComment == null ? "" : headComment.getComment());
    }
    
    public void setInlineComment(String comment) {
        if (comment == null || comment.length() == 0)
            this.inlineComment = null;
        else
            this.inlineComment = new InlineComment(comment);
    }
    
    public String getInlineComment() {
        return (inlineComment == null ? "" : inlineComment.getComment());
    }
    
    public void toFileString(final BufferedWriter bw, int depth) throws IOException {
        toFileString(bw, depth, Style.DEFAULT);
    }
    
    public void toFileString(final BufferedWriter bw, int depth, Style style) throws IOException {
        final StringBuilder sb = new StringBuilder(varname.length()+value.length()+6);
        
        if (headComment != null) {
            headComment.toFileString(bw, depth, style);
            bw.newLine();
            style.printTab(bw, depth);
        }
        
        sb.append(varname).append(style.getEqualsSign(depth));
        
        if (inQuotes)
            bw.write(sb.append('\"').append(value).append('\"').toString());
        else
            bw.write(sb.append(value).toString());
        
        if (inlineComment != null) {
            bw.write(' ');
            inlineComment.toFileString(bw, depth, style);
        }
    }
    
    @Override
    public String toString() {
//        System.out.println("Called ObjectVariable.toString()");
        return toString(Style.DEFAULT);
    }
    
    public String toString(Style s) {
        StringBuilder sb = new StringBuilder(varname).append(s.getEqualsSign(0));
        if (inQuotes)
            sb.append('\"').append(value).append('\"');
        else
            sb.append(value);
        
        if (inlineComment != null)
            sb.append(' ').append(inlineComment.toString());
        
        return sb.toString();
    }
    
    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        else if (!(obj instanceof ObjectVariable))
            return false;
        
        ObjectVariable ov = (ObjectVariable) obj;
        
        return (inQuotes == ov.inQuotes) && varname.equals(ov.varname) && value.equals(ov.value);
    }

    @Override
    public int hashCode() {
        int hash = 7;
        hash = 13 * hash + (this.varname != null ? this.varname.hashCode() : 0);
        hash = 13 * hash + (this.value != null ? this.value.hashCode() : 0);
        hash = 13 * hash + (this.inQuotes ? 1 : 0);
        return hash;
    }
    
    @Override
    public ObjectVariable clone() {
        final ObjectVariable retValue = new ObjectVariable(varname, value, inQuotes);
        
        if (headComment != null)
            retValue.setHeadComment(headComment.getComment());
        if (inlineComment != null)
            retValue.setInlineComment(inlineComment.getComment());
        
        return retValue;
    }

    public int compareTo(ObjectVariable o) {
        int ret = varname.compareTo(o.varname);
        if (ret != 0)
            return ret;
        
        return value.compareTo(o.value);
    }
    
}
