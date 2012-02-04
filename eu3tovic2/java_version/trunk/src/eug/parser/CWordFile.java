package eug.parser;

import eug.shared.GenericList;
import eug.shared.GenericObject;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.StringReader;
import javax.swing.JOptionPane;

/**
 * This class handles the parsing of .eug files. For most uses, you will want to
 * use {@link EUGFileIO} instead.
 * @author Michael Myers
 */
public class CWordFile {
    
    /**
     * The tokenizer used to scan the input file.
     */
    protected EUGScanner tokenizer;
    
    /**
     * The current token.
     */
    protected String m_word;
    
    private String lastComment;
    
    /**
     * The type of the {@link #m_word current token}.
     */
    protected TokenType tokenType;
    
    /**
     * The number of errors encountered.
     */
    protected int m_errors = 0;
    
    /**
     * The level of debugging, on a scale of 0 to 10.
     */
    private static final int debugLevel = 2;
    
    
    // Loading options
    
    protected ParserSettings settings;
    
//    /**
//     * Whether to display info on how long the loading took.
//     */
//    public static boolean timingInfo = false;
//
//    /**
//     * Whether to ignore comment tokens.
//     */
//    protected boolean commentsIgnored = true;
//
//    /**
//     * Whether lists are allowed to be parsed.
//     * @since EUGFile 1.02.00
//     */
//    protected boolean allowLists = true;
//
//    /**
//     * Whether single words (that are not part of a variable) can be parsed.
//     * Any such token will be given a default value of 1 (e.g., 1001 1002 will
//     * be parsed as 1001 = 1 1002 = 1).
//     * <p>
//     * This is only necessary for reading Victoria AI files, as far as I know.
//     * @since EUGFile 1.02.00
//     */
//    protected boolean allowSingleTokens = false;
//
//    /**
//     * Whether the strictest checking should be enabled.
//     * @since EUGFile 1.02.00
//     */
//    protected boolean warningsAreErrors = false;
//
//    /**
//     * Whether we will try to recover after, e.g., an extra '}'.
//     * @since EUGfile 1.06.00pre1
//     */
//    protected boolean tryToRecover = true;
    
    // End of loading options
    
    
    // used during loading
    private int newlinesSinceComment = 0;
    
    /**
     * Creates a new instance of CWordFile.
     */
    public CWordFile() {
        settings = ParserSettings.getDefaults();
    }
    
    public CWordFile(ParserSettings settings) {
        this.settings = settings;
    }
    
    //file handling
    /**
     * Open {@link #tokenizer the tokenizer} with the given filename.
     * @param filename the name of the input file.
     * @return <CODE>true</CODE> if the stream was successfully opened.
     */
    private boolean openInStream(String filename) {
        //try to open input file
        try {
            final File inFile = new File(filename);
            
            if (inFile.length() == 0) {
                tokenizer = new EUGScanner(new FileReader(inFile));
                return true;
            }
            
            tokenizer = new EUGScanner(new BufferedReader(new FileReader(inFile), (int)inFile.length()));
            tokenizer.setCommentsIgnored(settings.isIgnoreComments());
            return true;
        } catch (FileNotFoundException ex) {
            return false;
        }
    }
    
    /**
     * Open {@link #tokenizer the tokenizer} with a StringReader reading from the
     * given string.
     * @param string a string in the .eug format.
     */
    private void openStringStream(String string) {
        tokenizer = new EUGScanner(new StringReader(string));
    }
    
    /**
     * Close {@link #tokenizer the tokenizer}.
     */
    private void closeInStream() {
        if (tokenizer != null)
            tokenizer.close();
    }
    
    /**
     * Gets the next token from {@link #tokenizer the tokenizer}.
     * @see EUGScanner#nextToken()
     */
    protected final void getNextToken() {
        tokenType = tokenizer.nextToken();
        m_word = tokenizer.lastStr();
    }
    
    /**
     * Loads a {@link GenericObject} tree from the given string.
     * @param string a string containing an EUG tree.
     * @return the <code>GenericObject</code> tree loaded from the string, or
     * <code>null</code> if there was an error during loading.
     */
    public GenericObject loadFromString(String string) {
        final long startTime = System.nanoTime();
        
        //notify about loading
        assert debug("Loading from string.", 2);
        
        openStringStream(string);
        
        GenericObject root = null;
        
        try {
            root = new GenericObject();
            
            GenericObject curr = readObject(root);
            
            while (tokenType != TokenType.EOF) {
                curr = readObject(curr);
            }
        } catch (ParserException ex) {
            if (!settings.isTryToRecover())
                root = null;
        } finally {
            closeInStream();
        }
        
        if (m_errors > 0)
            System.out.println("There were " + m_errors + " errors during loading.");
        
        if (settings.isPrintTimingInfo())
            System.out.println("Loading took " + (System.nanoTime()-startTime) + " ns.\n");
        
        assert debug("The node read was:\n" + root, 6);
        
        return root;
    }
    
    /**
     * Loads a {@link GenericObject} tree from the given filename.
     * @param filename the name of the file containing an EUG tree (e.g., a savegame
     * or an event file).
     * @return the <code>GenericObject</code> tree loaded from the file, or
     * <code>null</code> if there was an error during loading.
     */
    public GenericObject load(final String filename) {
        final long startTime = System.nanoTime();
        
        if (!openInStream(filename))
            return null;
        
        //notify about loading
        if (settings.isPrintTimingInfo())
            System.out.println("Loading " + filename + ".");
        
        GenericObject root = null;
        
        try {
            root = new GenericObject();
            
            GenericObject curr = readObject(root);
            
            //reading loop (per line mainly)
            while (tokenType != TokenType.EOF) {
                curr = readObject(curr);
            }
        } catch (ParserException ex) {
            System.err.println(ex.getMessage());
            if (!settings.isTryToRecover())
                root = null;
        } finally {
            closeInStream();
        }
        
        //Tell some things about the current state:
        if (m_errors > 0)
            System.out.println("There were " + m_errors + " errors during loading.");
//        System.out.println("Read " + tokenizer.getCharsRead() + " bytes.");
        if (settings.isPrintTimingInfo())
            System.out.println("Loading took " + (System.nanoTime()-startTime) + " ns.\n");
        
        return root;
    }
    
    /**
     * Reads an object from the current stream.
     * @param current_node the node to read into, which cannot be <code>null</code>.
     * @return the next node to read into, which can be either the parameter,
     * the parameter's parent, or <code>null</code>.
     */
    private final GenericObject readObject(GenericObject current_node) {
        if (current_node == null) {
            throw new ParserException("node is null");
            //error("Cannot read into a null node!");
            //return null;
        }
        
        // Start by reading a word.
        getNextToken();
        
        switch (tokenType) {
            case IDENT:
                String name = m_word;
                getNextToken();
                
                // A little weirdness here, brought on by Java's lack of a
                // 'goto' keyword. Note that if we break out of the switch
                // normally, the while loop will also be broken. Its only
                // function is to restart the switch if we read a comment.
                findType: while (true) {
                    switch (tokenType) {
                        case COMMENT:
                            // fall through to NEWLINE
                        case NEWLINE:
                            do {
                                getNextToken();
                            } while (tokenType == TokenType.COMMENT || tokenType == TokenType.NEWLINE);
                            continue findType;
//                        case LIST:
//                            if (!settings.isAllowLists()) {
//                                warn("Read illegal token: "+tokenizer.getList());
//                                break;
//                            }
//
//                            if (settings.isIgnoreComments()) {
//                                current_node.addList(name, tokenizer.getList());
//                            } else {
//                                current_node.addList(name, tokenizer.getList(),
//                                        lastComment, readInlineComment());
//                            }
//                            lastComment = null;
//                            break;
                        case ULSTRING:
                        case DLSTRING:
                            String val = m_word;
                            boolean quotes = (tokenType == TokenType.DLSTRING);
                            
                            current_node.addString(name, val, quotes,
                                    lastComment, readInlineComment());
                            
                            lastComment = null;
                            break;
                        case LBRACE:
                            // Lookahead
                            tokenizer.setCommentsIgnored(true);
                            TokenType type = tokenizer.nextToken();
                            tokenizer.pushBack();
                            tokenizer.setCommentsIgnored(settings.isIgnoreComments());
                            if (type == TokenType.DLSTRING || type == TokenType.ULSTRING) {
                                // it must be a list
                                if (!settings.isAllowLists()) {
                                    warn("Read list when lists are not allowed");
                                    break;
                                }
                                current_node = tryToReadList(current_node, name);
                                lastComment = null;
                            } else {
                                GenericObject tmpObj = current_node.createChild(name);
                                if (!settings.isIgnoreComments())
                                    tmpObj.setHeadComment(lastComment);
                                lastComment = null;
                                current_node = tmpObj;
                            }
                            break;
                        default:
                            warn("Unexpected token type: " + tokenType);
                            break;
                    }
                    // If we get here normally, we shouldn't loop again
                    break;
                }
                
                break;
            case RBRACE:     // Reached end of a node.
                if (current_node.getParent() == null) {
                    throw new ParserException("Unmatched right brace on line " +
                            tokenizer.getLine() + ", column " +
                            tokenizer.getColumn());
                }
                if (!settings.isIgnoreComments()) {
                    current_node.setInlineComment(readInlineComment());
                }
                current_node = current_node.getParent();
                lastComment = null;
                break;
            case LBRACE:     // Create nameless node
                current_node = current_node.createChild("");
                break;
            case EOF:
                if (current_node.isRoot() || settings.isTryToRecover())
                    current_node = null;
                else
                    throw new ParserException("Node " + current_node.name + " does not terminate");
                break;
            case COMMENT:
                // Here I'm trying to avoid the problem of file header comments
                // being merged into event header comments. I keep a record of
                // how many newline characters have been read since the last
                // comment ended. If >= 2, the old comment is added to the root
                // node.
                if (lastComment == null || lastComment.length() == 0) {
                    lastComment = m_word;
                    newlinesSinceComment = 0;
                } else if (newlinesSinceComment >= 2) {
                    current_node.getRoot().addGeneralComment(lastComment);
                    lastComment = m_word;
                    // Reset the newline count.
                    newlinesSinceComment = 0;
                } else {
                    // Append to previous comment string.
                    lastComment += "\n" + m_word;
                    newlinesSinceComment = 0;
                }
                break;
            case ULSTRING:
                // Changed in 1.01.00 to allow for Victoria AI files with
                // things like:
                // technology = {
                //      preference = { 1001 1002 3301 = 20 }
                // }
                // So, we add the unmatched string with a default value of 1.
                
                // Changed in 1.02.00 to check allowSingleTokens
                if (settings.isAllowSingleTokens()) {
                    current_node.addString(m_word, "1", false, lastComment,
                            readInlineComment());
                } else {
                    warn("Warning: Illegal string: " + m_word);
                }
                break;
            case DLSTRING:
                // Shouldn't happen.
                warn("Warning: Illegal string: \"" + m_word + "\"");
                break;
            case NEWLINE:
                // Only used for matching header comments to objects.
                newlinesSinceComment++;
                break;
            default:
                // Shouldn't happen.
                System.err.println("Token type: " + tokenType + "\nm_word: " + m_word);
                break;
        }
        
        return current_node;
    }
    
    /**
     * Tries to read a list as a child of the given object with the given name.
     * If an invalid token is read, the list is converted into an object and
     * all list entries are turned into variables with a value of 1.
     * @return the node that should be read into after this method has finished.
     */
    private GenericObject tryToReadList(final GenericObject parent, final String name) {
        final GenericList list = parent.createList(name);
        
        if (!settings.isIgnoreComments() && lastComment != null)
            list.setHeaderComment(lastComment);
        
        boolean ok = true;
        readlist: while (true) {
            getNextToken();
            switch (tokenType) {
                case ULSTRING:
                    list.add(m_word, false);
                    break;
                case DLSTRING:
                    list.add(m_word, true);
                    break;
                case RBRACE:
                    break readlist;
                case COMMENT:
                case NEWLINE:
                    break;
                default:
//                    warn("Unexpected list item at line " + tokenizer.getLine() +
//                            " column " + tokenizer.getColumn());
                    ok = false;
                    break readlist;
            }
        }
        
        // There are two ways that the loop could be broken:
        // 1. An invalid token (IDENT) was read.
        //    -> Turn the list into an object and return it.
        // 2. A right brace was read.
        //    -> Check for an inline comment and return the parent.
        
        if (!ok) {
            GenericObject child = parent.createChild(name);
            for (String var : list) {
                if (var.contains(" ")) {
                    child.addString("\"" + var + "\"", "1");
                } else {
                    child.addString(var, "1");
                }
            }
            if (!"".equals(list.getHeaderComment()))
                child.setHeadComment(list.getHeaderComment());
            parent.removeList(list);
            tokenizer.pushBack(); // because we didn't do anything with the invalid token
            return child;
        } else {
            if (!settings.isIgnoreComments()) {
                list.setInlineComment(readInlineComment());
                lastComment = null;
            }
            return parent;
        }
    }
    
    /**
     * Read a comment and then a newline, if possible. If there is no comment
     * (or if {@link #commentsIgnored}), returns <code>null</code>.
     */
    private String readInlineComment() {
        if (settings.isIgnoreComments())
            return null;
        
        String comment = null;
        
        getNextToken();
        
        if (tokenType == TokenType.COMMENT) {
            comment = m_word;   // Got the comment; now look for a newline.
//            System.out.println(comment);
            
            getNextToken();
            
            if (tokenType != TokenType.NEWLINE) {
                // Anything other than a newline shouldn't be used here.
//                System.out.println(tokenType);
                tokenizer.pushBack();
            }
        } else if (tokenType == TokenType.NEWLINE) {
            return null;
        } else
            tokenizer.pushBack();
        
        return comment;
    }
    
//    /**
//     * Tries to read a list of strings. If an identifier is encountered, returns
//     * <code>null</code>.
//     */
//    private List<String> readList() {
//        List<String> ret = new ArrayList<String>();
//        outer: while (true) {
//            getNextToken();
//            switch (tokenType) {
//                case COMMENT:
//                case NEWLINE:
//                    continue outer;
//                case IDENT:
//
//            }
//        }
//    }
    
    /**
     * Displays an error dialog with the given text, and also prints it to
     * System.err.
     * @param text the error message to display.
     */
    private void error(final String text) {
        System.out.println(text); //+" on line "+tokenizer.getLine()+", column "+tokenizer.getColumn());
        JOptionPane.showMessageDialog(null, text, "Error", JOptionPane.ERROR_MESSAGE);
        m_errors++;
    }
    
    /**
     * Prints a warning message with the given text.
     * If {@link #warningsAreErrors}, this method will also throw a
     * {@link ParserException}, so any method that calls this should check for one.
     * @param text the warning text to display.
     * @since EUGFile 1.02.00
     */
    private void warn(final String text) {
        System.err.println(text);
        if (settings.isWarningsAreErrors())
            throw new ParserException(text);
    }
    
    /**
     * Prints the given text to standard out if <code>level</code> &lt;=
     * {@link #debugLevel}.
     * @param text the text to print out.
     * @param level the level of importance.
     * @return <code>true</code>. This is only so that this method can be used
     * in assertions.
     */
    private static boolean debug(final String text, int level) {
        if (level <= debugLevel) {
            System.out.println("DEBUG: " + text);
        }
        return true;
    }
    
    public void setParserSettings(ParserSettings settings) {
        this.settings = settings;
    }
    
    public ParserSettings getParserSettings() {
        return settings.clone();
    }
    
    public boolean commentsIgnored() {
        return settings.isIgnoreComments();
    }
    
    public void setCommentsIgnored(boolean ignored) {
        settings.setIgnoreComments(ignored);
    }
    
    public boolean allowsLists() {
        return settings.isAllowLists();
    }
    
    public void setAllowLists(boolean allowLists) {
        settings.setAllowLists(allowLists);
    }
    
    public boolean allowsSingleTokens() {
        return settings.isAllowSingleTokens();
    }
    
    public void setAllowSingleTokens(boolean allowSingleTokens) {
        settings.setAllowSingleTokens(allowSingleTokens);
    }
    
    public boolean warningsAreErrors() {
        return settings.isWarningsAreErrors();
    }
    
    public void setWarningsAreErrors(boolean warningsAreErrors) {
        settings.setWarningsAreErrors(warningsAreErrors);
    }
    
    public boolean isTryToRecover() {
        return settings.isTryToRecover();
    }
    
    public void setTryToRecover(boolean tryToRecover) {
        settings.setTryToRecover(tryToRecover);
    }
}