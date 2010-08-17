/*
 * ParserSettings.java
 *
 * Created on July 4, 2007, 11:07 AM
 */

package eug.parser;

/**
 * Encapsulates a number of parameters used during parsing.
 * <p>
 * The only way of creating an instance is to call one of the static factory
 * methods {@link #getDefaults()}, {@link #getNoCommentSettings()}, and
 * {@link #getStrictSettings()}. All of these return clones of the actual static
 * objects, so it is perfectly legitimate (and expected, although not always
 * necessary) to customize the returned object.
 * <p>
 * A common usage is when a program wants to load things silently. This is
 * easily accomplished by keeping a field holding
 * <code>ParserSettings.getDefaults().setPrintTimingInfo(false)</code> and
 * passing it to any <code>EUGFileIO.load()</code> methods used.
 * @author Michael Myers
 * @see EUGFileIO#load(String, ParserSettings)
 * @see EUGFileIO#load(File, ParserSettings)
 * @see EUGFileIO#loadFromString(String, ParserSettings)
 * @since EUGFile 1.06.00pre1
 */
public final class ParserSettings implements Cloneable, java.io.Serializable {
    
    private static final long serialVersionUID = 1L;
    
    /**
     * Indicates whether or not lists should be allowed.
     * <p>
     * The default value is <code>true</code>.
     */
    private boolean allowLists;
    
    /**
     * Indicates whether or not single words should be allowed.
     * <p>
     * The default value is <code>false</code>.
     */
    private boolean allowSingleTokens;
    
    /**
     * Indicates whether or not comments should be ignored.
     * <p>
     * The default value is <code>false</code>.
     */
    private boolean ignoreComments;
    
    /**
     * Indicates whether to display info on how long the loading took.
     * <p>
     * The default value is <code>true</code>.
     */
    private boolean printTimingInfo;
    
    /**
     * Indicates whether or not the parser should try to recover after, e.g.,
     * an extra '}'.
     * <p>
     * The default value is <code>true</code>.
     */
    private boolean tryToRecover;
    
    /**
     * Indicates whether or not warnings should be treated as errors.
     * <p>
     * The default value is <code>false</code>.
     */
    private boolean warningsAreErrors;
    
    /** Creates a new instance of ParserSettings */
    private ParserSettings() {
        setAllowLists(true);
        setAllowSingleTokens(false);
        setIgnoreComments(false);
        setPrintTimingInfo(true);
        setTryToRecover(true);
        setWarningsAreErrors(false);
    }
    
    /**
     * Indicates whether or not lists should be allowed.
     * <p>
     * The default value is <code>true</code>.
     * @return a reference to this object.
     */
    public ParserSettings setAllowLists(boolean allow) {
        allowLists = allow;
        return this;
    }
    
    /**
     * Indicates whether or not single words should be allowed.
     * <p>
     * The default value is <code>false</code>.
     * @return a reference to this object.
     */
    public ParserSettings setAllowSingleTokens(boolean allow) {
        allowSingleTokens = allow;
        return this;
    }
    
    /**
     * Indicates whether or not comments should be ignored.
     * <p>
     * The default value is <code>false</code>.
     * @return a reference to this object.
     */
    public ParserSettings setIgnoreComments(boolean ignore) {
        ignoreComments = ignore;
        return this;
    }
    
    /**
     * Indicates whether to display info on how long the loading took.
     * <p>
     * The default value is <code>true</code>.
     * @return a reference to this object.
     */
    public ParserSettings setPrintTimingInfo(boolean info) {
        printTimingInfo = info;
        return this;
    }
    
    /**
     * Indicates whether or not the parser should try to recover after, e.g.,
     * an extra '}'.
     * <p>
     * The default value is <code>true</code>.
     * @return a reference to this object.
     */
    public ParserSettings setTryToRecover(boolean recover) {
        tryToRecover = recover;
        return this;
    }
    
    /**
     * Indicates whether or not warnings should be treated as errors.
     * <p>
     * The default value is <code>false</code>.
     * @return a reference to this object.
     */
    public ParserSettings setWarningsAreErrors(boolean errors) {
        warningsAreErrors = errors;
        return this;
    }

    public boolean isAllowLists() {
        return allowLists;
    }

    public boolean isAllowSingleTokens() {
        return allowSingleTokens;
    }

    public boolean isIgnoreComments() {
        return ignoreComments;
    }

    public boolean isPrintTimingInfo() {
        return printTimingInfo;
    }

    public boolean isTryToRecover() {
        return tryToRecover;
    }

    public boolean isWarningsAreErrors() {
        return warningsAreErrors;
    }
    
    
    private static final ParserSettings defaults = new ParserSettings();
    private static final ParserSettings noCommentSettings = new ParserSettings().setIgnoreComments(true);
    private static final ParserSettings strictSettings = new ParserSettings().setTryToRecover(false).setWarningsAreErrors(true);
    private static final ParserSettings quietSettings = new ParserSettings().setPrintTimingInfo(false);
    
    
    /**
     * Returns the default parser settings. Values are as shown in the
     * getter/setter documentation.
     * @return the default parser settings.
     */
    public static ParserSettings getDefaults() {
        return defaults.clone();
    }
    
    /**
     * Returns an instance of <code>ParserSettings</code> that is equal to the
     * defaults, except that <code>ignoreComments</code> is <code>true</code>.
     * @return a <code>ParserSettings</code> object that ignores comments.
     */
    public static ParserSettings getNoCommentSettings() {
        return noCommentSettings.clone();
    }
    
    /**
     * Returns an instance of <code>ParserSettings</code> that is equal to the
     * defaults, except that <code>tryToRecover</code> is <code>false</code> and
     * <code>warningsAreErrors</code> is <code>true</code>.
     * @return a set of strict parser settings.
     */
    public static ParserSettings getStrictSettings() {
        return strictSettings.clone();
    }
    
    /**
     * Returns an instance of <code>ParserSettings</code> that is equal to the
     * defaults, except that <code>printTimingInfo</code> is <code>false</code>.
     * @return a <code>ParserSettings</code> object that does not print timing
     * data to the console.
     */
    public static ParserSettings getQuietSettings() {
        return quietSettings.clone();
    }
    
    
    @Override
    public ParserSettings clone() {
        try {
            return (ParserSettings) super.clone(); // we only have booleans, which should clone OK
        } catch (CloneNotSupportedException ex) {
            throw new InternalError(ex.getMessage());
        }
    }
    
    @Override
    public boolean equals(Object o) {
        if (!(o instanceof ParserSettings))
            return false;
        
        ParserSettings other = (ParserSettings) o;
        
        return (allowLists == other.allowLists) &&
                (allowSingleTokens == other.allowSingleTokens) &&
                (ignoreComments == other.ignoreComments) &&
                (printTimingInfo == other.printTimingInfo) &&
                (tryToRecover == other.tryToRecover) &&
                (warningsAreErrors == other.warningsAreErrors);
    }

    @Override
    public int hashCode() {
        int hash = 7;
        hash = 53 * hash + (this.allowLists ? 1 : 0);
        hash = 53 * hash + (this.allowSingleTokens ? 1 : 0);
        hash = 53 * hash + (this.ignoreComments ? 1 : 0);
        hash = 53 * hash + (this.printTimingInfo ? 1 : 0);
        hash = 53 * hash + (this.tryToRecover ? 1 : 0);
        hash = 53 * hash + (this.warningsAreErrors ? 1 : 0);
        return hash;
    }
    
}
