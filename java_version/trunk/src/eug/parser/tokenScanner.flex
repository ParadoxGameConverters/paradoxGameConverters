/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * TokenScanner.java                                                         *
 * This file is generated from tokenScanner.flex. Please edit that file if   *
 * you wish to make changes.                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

package eug.parser;

/**
 * This class is almost the same as {@link EUGScanner}, but it doesn't handle
 * lists or identifiers. Rather, each token in the list is passed directly.
 * This is better for syntax highlighting, but perhaps not as good for parsing
 * data.
 * @author Michael Myers
 * @since EUGFile 1.06.00pre1
 */
%%

%{
    /** Save the last token type for use by {@link #lastStr}. */
    private TokenType lastType;


    private static final java.util.regex.Pattern whitespacePattern =
            java.util.regex.Pattern.compile("\\s+");

    /**
     * Convenience method for getting an unquoted string from a quoted one.
     * All whitespace gaps are replaced with a single " ", so quoted strings
     * can span multiple lines.
     */
    private final String getDLString() {
        return whitespacePattern.matcher(yytext().substring(1,yylength()-1).trim()).replaceAll(" ");
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
     */
    public TokenType lastToken() {
        return lastType;
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
            default:
                return "";
        }
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
     */
    public void reset(java.io.Reader in) {
        yyreset(in);
    }

    /**
     * Resets the scanner to read from a new input stream.
     * Does not close the old reader.
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
     */
    public int getTokenStart() {
        return yychar;
    }

    /**
     * Returns the number of characters contained in the token. Useful for
     * syntax highlighting.
     */
    public int getTokenSize() {
        return yylength();
    }

    /**
     * Returns the index of the end of the token. Useful for
     * syntax highlighting.
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
%class TokenScanner

%apiprivate

%function next

%type TokenType

/* Count lines, columns, and characters */
%line
%column
%char

/* %full */
%unicode

/* This scanner simply breaks the input into tokens. No extra states are needed. */

/* %switch */
%table

%standalone

ALPHA                       = [[:letter:]_\[\]\-'´¨,]   //[A-Za-zÀ-ÿ_\[\]\-'´¨]
DIGIT                       = [0-9\.\-\+]
ALNUM                       = {ALPHA}|{DIGIT}

NONNEWLINE_WHITE_SPACE_CHAR = [\ \t\b\012]
NEWLINE                     = \r|\n|\r\n
NONNEWLINE                  = [^\r\n]

COMMENT                     = "#" {NONNEWLINE}* /* {NEWLINE}? */
QUOTED_STR                  = \" [^\"]* \"

/* Note: UNQUOTED_STR matches numbers, too. */
UNQUOTED_STR                = {ALNUM}+

%% 

/***** Globals *****/

{NONNEWLINE_WHITE_SPACE_CHAR}+  { break; }

{NEWLINE}               { return TokenType.NEWLINE; }

{COMMENT}               { return TokenType.COMMENT; }

{QUOTED_STR}            { return TokenType.DLSTRING; }

{UNQUOTED_STR}          { return TokenType.ULSTRING; }

"="                     { return TokenType.EQUALS; }
"{"                     { return TokenType.LBRACE; }
"}"                     { return TokenType.RBRACE; }


/* Catch-all: Print error message and try again. */
.                       { break; /*badChar(yycharat(0)); return next();*/ }

/***** End Globals *****/
