/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * EUGScanner.java                                                           *
 * This file is generated from scanner.flex. Please edit that file if you    *
 * wish to make changes.                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

package eug.parser;

/**
 * This is a simple scanner that does no error checking. If there is a problem
 * (e.g., an "=" in a list), the parser that uses this class is responsible for
 * recovering.
 * <p>
 * To use this class, the following sequence is typical:
 * <ol>
 * <li>Call the {@link EUGScanner(java.io.Reader) constructor} with a
 *      <code>Reader</code>.<br> Note that it is also possible to use an
 *      <code>InputStream</code>, but it will be wrapped in an
 *      <code>InputStreamReader</code>.<br> Note also that this class uses a
 *      buffer internally, so a <code>BufferedReader</code> is not necessary.
 * <li>Loop with calls to {@link #nextToken()}, performing whatever parsing is
 *      necessary.<br>The text read (minus any extraneous characters such as '#'
 *      or '"') can be retrieved through {@link lastStr()}.
 * <li>When finished, call {@link #close()}.
 * </ol>
 * <h4>Comment Handling:</h4>
 * Comments can be enabled or disabled through
 * {@link #setCommentsIgnored(boolean)}. If they are disabled,
 * <code>nextToken()</code> will never return {@link TokenType#COMMENT}.
 * @author Michael Myers
 */
%%

%{
    /** Save the last token type for use by {@link #lastStr}. */
    private TokenType lastType;

    private boolean commentsIgnored = false;


    private static final java.util.regex.Pattern whitespacePattern =
            java.util.regex.Pattern.compile("\\s+");

    /**
     * Convenience method for getting an unquoted string from a quoted one.
     * All whitespace gaps are replaced with a single " ", so quoted strings
     * can span multiple lines.
     */
    private final String getDLString() {
        return whitespacePattern.matcher(yytext().trim().substring(1,yylength()-1).trim()).replaceAll(" ");
    }

    /**
     * Convenience method for extracting the comment text from a comment string.
     * This method simply replaces all occurrences of the comment start
     * character ('#') with ' ', then trims whitespace off.
     */
    private final String getComment() {
        int lastIdxOfHash = 0;
        for (int i = 0; i < yylength(); i++) {
            if (yycharat(i) == '#')
                lastIdxOfHash = i;
            else
                break;
        }
        return yytext().substring(lastIdxOfHash+1).trim();
    }

    private static final java.util.regex.Pattern commentPattern =
            java.util.regex.Pattern.compile("#[^\r\n]*(\r|\n|\r\n)");

    /** Convenience method for getting an identifier from a string such as "tag =". */
    private final String getIdent() {
        // Remove comments, remove the "=", then trim whitespace.
        return commentPattern.matcher(yytext()).replaceAll("").replace('=', ' ').replace('"', ' ').trim();
    }

    private void badChar(char c) {
        System.err.println("Illegal character: \'" + c +
            "\' (#" + Integer.toHexString((int)c) + 
            ") on line "+yyline+", column "+yycolumn);
    }

    /**
     * Reads the next token from the input reader.
     * @return the type of token that was just read. The actual token text
     * (if the token was of type <code>DLSTRING</code>, <code>ULSTRING</code>,
     * or <code>COMMENT</code> [if comments are not ignored]) can be accessed
     * from {@link #lastStr()}.
     */
    public TokenType nextToken() {
        try {
            return (lastType = next());
        } catch (java.io.IOException ex) {
            return (lastType = TokenType.EOF);
        }
    }

    /**
     * Returns the last token type read from the input.
     * @since EUGFile 1.06.00pre1
     */
    public TokenType lastToken() {
        return lastType;
    }

    /**
     * Call this method with <code>true</code> to keep comment tokens from being
     * parsed by {@link nextToken()}.
     * @param ignored <code>true</code> to ignore comments, <code>false</code>
     * to allow comments to be read.
     */
    public void setCommentsIgnored(boolean ignored) {
        commentsIgnored = ignored;
    }

    /**
     * Returns the last string read by the scanner, if the last token included
     * text.
     * @return the last string read by the scanner, stripped of extraneous
     * characters such as '#' and '"'.
     */
    public String lastStr() {
        switch(lastType) {
            case ULSTRING:
                return yytext();
            case DLSTRING:
                return getDLString();
            case COMMENT:
                return getComment();
            case IDENT:
                return getIdent();
            default:
                return "";
        }
    }

    /**
     * Returns the current list in array form. All comments are stripped out.
     * @see #getList()
     * @since EUGFile 1.01.00
     */
    public String[] getArray() {
        if (lastType != TokenType.LIST) {
            System.err.println("Shouldn't be trying to get a list from token type " + lastType);
            return new String[] { };
        }

        return ListScanner.parseArray(yytext().substring(yytext().indexOf('{') + 1, yytext().indexOf('}')));
    }
    /**
     * Returns the current list in the form of a <code>List&lt;String&gt;</code>.
     * All comments are stripped out.
     * @see #getArray()
     */
    public java.util.List<String> getList() {
        if (lastType != TokenType.LIST) {
            System.err.println("Shouldn't be trying to get a list from token type " + lastType);
            return new java.util.ArrayList<String>();
        }

        return ListScanner.parseList(yytext().substring(yytext().indexOf('{') + 1, yytext().indexOf('}')));
    }

    /**
     * Close the scanner's reader. Any <code>IOException</code> will be caught
     * and printed to standard error.
     */
    public void close() {
        try {
            yyclose();
        } catch (java.io.IOException ex) {
            ex.printStackTrace();
        }
    }

    /**
     * Resets the scanner to read from a new input stream.
     * Does not close the old reader.
     * @since EUGFile 1.06.00pre1
     */
    public void reset(java.io.Reader in) {
        yyreset(in);
    }

    /**
     * Resets the scanner to read from a new input stream.
     * Does not close the old reader.
     * @since EUGFile 1.06.00pre1
     */
    public void reset(java.io.InputStream in) {
        yyreset(new java.io.InputStreamReader(in));
    }

    /**
     * Push the current token back into the stream.
     */
    public void pushBack() {
        yypushback(yylength());
        yybegin(YYINITIAL);
    }

    /**
     * Returns the number of newlines encountered up to the start of the token.
     */
    public int getLine() {
        return yyline;
    }

   /**
    * Returns the number of characters from the last newline up to the start of
    * the token.
    */
    public int getColumn() {
        return yycolumn;
    }

    /** 
     * Returns the number of characters up to the start of the token.
     * @see #getTokenStart()
     */
    public int getCharsRead() {
        return yychar;
    }

    /**
     * Returns the index of the start of the token. Useful for
     * syntax highlighting.
     * @see #getCharsRead()
     * @since EUGFile 1.06.00pre1
     */
    public int getTokenStart() {
        return yychar;
    }

    /**
     * Returns the number of characters contained in the token. Useful for
     * syntax highlighting.
     * @since EUGFile 1.06.00pre1
     */
    public int getTokenSize() {
        return yylength();
    }

    /**
     * Returns the index of the end of the token. Useful for
     * syntax highlighting.
     * @since EUGFile 1.06.00pre1
     */
    public int getTokenEnd() {
        return yychar + yylength();
    }
%} 

%eofval{
    return TokenType.EOF;
%eofval}


%public
%final
%class EUGScanner

%apiprivate

%function next

%type TokenType

/* Count lines, columns, and characters */
%line
%column
%char

/* %full */
%unicode

/* This scanner currently does no error checking. If and when error checking
    is implemented, more states would be added. */

%state HAS_IDENT


/* %switch */
%table

%standalone

ALPHA                       = [[:letter:]_\[\]\-'´¨,]    //[A-Za-zÀ-ÿ_\[\]\-'´¨]
DIGIT                       = [0-9\.\-\+]
ALNUM                       = {ALPHA}|{DIGIT}

NONNEWLINE_WHITE_SPACE_CHAR = [\ \t\b\012]
NEWLINE                     = \r|\n|\r\n
NONNEWLINE                  = [^\r\n]
WHITE_SPACE_CHAR            = [\n\r\ \t\b\012]
NON_OPERATOR                = [^(=|{|}|#)]

COMMENT                     = "#" {NONNEWLINE}*
COMMENT_NEWLINE             = "#" {NONNEWLINE}* {NEWLINE}
QUOTED_STR                  = \" [^\"]* \"

/* Note: UNQUOTED_STR matches numbers, too. */
UNQUOTED_STR                = {ALNUM}+

/* Note: The Ident macro will slow things down. */
Ident                       = ({UNQUOTED_STR} | {QUOTED_STR}) ({WHITE_SPACE_CHAR}* {COMMENT_NEWLINE}* {WHITE_SPACE_CHAR}*)* "=" {NONNEWLINE_WHITE_SPACE_CHAR}*

/* List-detection is somewhat complicated. We want to detect all bracket-
 * delimited blocks such that
 * 1. There is no non-commented equals sign in the block.
 * 2. There is at least one non-blank, non-commented character in the block.
 *
 * What makes it difficult is that sometimes comment characters occur inside
 * quote-delimited strings, such as:
 *
 * monarch_names = {
 *      "Yusuf #0" = 20
 *      "Ibrahim #0" = 40
 *      "Ali #0" = 20
 *      "Mohammed #0" = 20
 *      "Sikandi #0" = 40
 * }
 *
 * In this example, all equals signs occur after comment characters, which
 * fools the parser into thinking that it is a list (which causes errors every
 * time one of the equals signs is read).
 */
//LIST                        = "{" {NON_OPERATOR}* {ALNUM} {NON_OPERATOR}* "}" {NONNEWLINE_WHITE_SPACE_CHAR}*
//LIST                        = "{"
//                                ({COMMENT_NEWLINE}* {NON_OPERATOR}*)*
//                                (({ALNUM} {NON_OPERATOR}* {COMMENT_NEWLINE}*)|({COMMENT_NEWLINE}* {NON_OPERATOR}* {ALNUM}*))+
//                                ({NON_OPERATOR}* {COMMENT_NEWLINE}*)*
//                            "}"
//                            {NONNEWLINE_WHITE_SPACE_CHAR}*

%% 

/***** Globals *****/

{NONNEWLINE_WHITE_SPACE_CHAR}+  { break; }

{NEWLINE}                 { if (commentsIgnored) return next(); else return TokenType.NEWLINE; }

{COMMENT}                 { if (commentsIgnored) return next(); else return TokenType.COMMENT; }

//{QUOTED_STR}            { return TokenType.DLSTRING; }

//{UNQUOTED_STR}          { return TokenType.ULSTRING; }

//{Ident}                 { return TokenType.IDENT; }

//{LIST}                  { return TokenType.LIST; }


//"="                     { return TokenType.EQUALS; }
//"{"                     { return TokenType.LBRACE; }
//"}"                     { return TokenType.RBRACE; }


///* Catch-all: Print error message and try again. */
//.                       { badChar(yycharat(0)); return next(); }

/***** End Globals *****/

<YYINITIAL> {
    {Ident}                 { yybegin(HAS_IDENT); return TokenType.IDENT; }

    {QUOTED_STR}            { return TokenType.DLSTRING; }
    {UNQUOTED_STR}          { return TokenType.ULSTRING; }

    /*
     * We are forced to include the "{" rule because of things like
     * <code>
     * attackers = { { type = 4712 id = 15897830 } { type = 4712 id = 16334940 } } 
     * </code>
     */
    "{" {NONNEWLINE_WHITE_SPACE_CHAR}* { return TokenType.LBRACE; }
    "}" {NONNEWLINE_WHITE_SPACE_CHAR}* { return TokenType.RBRACE; }
    .                       { System.err.print("YYINITIAL: "); badChar(yycharat(0)); return next(); }
}


<HAS_IDENT> {
    "{" {NONNEWLINE_WHITE_SPACE_CHAR}* { yybegin(YYINITIAL); return TokenType.LBRACE; }
    {QUOTED_STR}            { yybegin(YYINITIAL); return TokenType.DLSTRING; }
    {UNQUOTED_STR}          { yybegin(YYINITIAL); return TokenType.ULSTRING; }
    //{LIST}                  { yybegin(YYINITIAL); return TokenType.LIST; }
    .                       { System.err.print("HAS_IDENT: "); badChar(yycharat(0)); return next(); }
}

