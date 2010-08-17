/*
 * Comment.java
 *
 * Created on July 11, 2006, 10:21 PM
 */

package eug.shared;

import java.lang.reflect.InvocationTargetException;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Pattern;

/**
 * Abstract class that encapsulates a comment. The exact format of the comment is
 * left to subclasses, but the {@link #commentChar comment-start character} is not.
 * @author Michael Myers
 */
public abstract class Comment implements WritableObject, Cloneable {
    
    /**
     * The character that must start all comments when printed out.
     */
    protected static final String commentChar = "#";
    
    /**
     * The actual comment text.
     */
    protected String comment;
    
    /**
     * The pattern used to split comments into lines.
     */
    protected static final Pattern newLinePattern = Pattern.compile("(\\n|\\r\\n)");
    
    /**
     * Creates a new <code>Comment</code> with the given text.
     * @param comment the comment text.
     */
    public Comment(String comment) {
        this.comment = (comment == null ? "" : comment);
    }

    /**
     * Gets the comment text.
     * @return the comment text.
     */
    public String getComment() {
        return comment;
    }

    /**
     * Sets the comment text to the given string.
     * @param comment the new comment text.
     */
    public void setComment(String comment) {
        this.comment = comment;
    }
    
    /**
     * Appends the given string to the comment text. If the comment was empty,
     * it is set to the given string.
     * <p>
     * Note that different subclasses of <code>Comment</code> may print
     * multiline comments in different ways.
     * @param toAdd a <code>String</code> to append to the comment.
     * @since EUGFile 1.01.03
     */
    public void appendComment(String toAdd) {
        if (toAdd.length() == 0)
            return;
        
        if (comment.length() == 0)
            comment = toAdd;
        else
            comment += "\n" + toAdd;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == null)
            return false;
        
        return getClass() == obj.getClass() &&  // check for subclasses
                comment.equals(((Comment)obj).comment);
    }

    @Override
    public int hashCode() {
        int hash = 7;
        hash = 79 * hash + (this.comment != null ? this.comment.hashCode() : 0);
        return hash;
    }
}
