/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ListScanner.java                                                          *
 * This file is generated from listScanner.flex. Please edit that file if    *
 * you wish to make changes.                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

package eug.parser;

import java.util.List;
import java.util.ArrayList;

/**
 * This scanner parses list items from something like:
 * <code>
 *  "New Zealand"#Important
 *              Ontario
 *              "Québec" "British Columbia"
 * </code>
 * <p>
 * This is an internal class, meant for use only by {@link EUGScanner}.
 * @author Michael Myers
 * @since EUGFile 1.01.00
 */
%%

%{
    private static ListScanner scanner;

    /**
     * Returns the shared instance of <code>ListScanner</code>. If it has
     * previously been created, it is reset with the given reader.
     * <p>
     * This method is used to keep from creating a new <code>ListScanner</code>
     * every time a list is read. This greatly reduces overhead in creating
     * the character tables, etc.
     * @since EUGFile 1.03.00
     */
    private static ListScanner getScanner(java.io.Reader reader) {
        if (scanner == null) {
            scanner = new ListScanner(reader);
        } else {
            scanner.yyreset(reader);
        }
        return scanner;
    }
            
    /**
     * Parses an array of strings from the given string. This is like
     * <code>str.split("\\s+")</code> except that it allows for quoted strings
     * with whitespace enclosed.
     * @param list the whitespace-delimited list of strings. Note that quoted
     * and unquoted strings can be mixed freely.
     * @return the array of strings.
     * @see #parseList(String)
     */
    static String[] parseArray(String list) {
        try {
            return getScanner(new java.io.StringReader(list)).parseArray();
        } catch (Exception ex) {
            System.err.println("Error reading list!");
            ex.printStackTrace();
            return null;
        }
    }

    /**
     * Parses a list of strings from the given string. This is like
     * <code>str.split("\\s+")</code> except that it allows for quoted strings
     * with whitespace enclosed and it returns a list.
     * @param list the whitespace-delimited list of strings. Note that quoted
     * and unquoted strings can be mixed freely.
     * @return the list of strings.
     * @see #parseArray(String)
     */
    static List<String> parseList(String list) {
        try {
            return getScanner(new java.io.StringReader(list)).parseList();
        } catch (Exception ex) {
            System.err.println("Error reading list!");
            ex.printStackTrace();
            return null;
        }
    }

    private String[] parseArray() throws java.io.IOException {
        final List<String> list = new ArrayList<String>();
        String tmp;
        while ((tmp = yylex()) != null)
            list.add(tmp);

        return list.toArray(new String[list.size()]);
    }

    private List<String> parseList() throws java.io.IOException {
        final List<String> list = new ArrayList<String>();
        String tmp;
        while ((tmp = yylex()) != null)
            list.add(tmp);

        return list;
    }

    private void badChar(char c) {
        System.err.println("Illegal list character: \'" + c +
            "\' (#" + Integer.toHexString((int)c) + ")");
    }
%}


%eofval{
    return null;
%eofval}

%final
%class ListScanner

%apiprivate

%type String

%unicode

/* %switch */
%table

ALPHA                       = [[:letter:]_\[\]\-'´¨,]
DIGIT                       = [0-9\.\-]

WHITE_SPACE_CHAR            = [\r\n\ \t\b\012]
//NEWLINE                     = [\r\n]
NONNEWLINE                  = [^\r\n]

COMMENT                     = "#" {NONNEWLINE}* /* NEWLINE */
QUOTED_STR                  = \" [^\"]* \"

/* Note: UNQUOTED_STR matches numbers, too. */
UNQUOTED_STR                = ({ALPHA}|{DIGIT})+


%% 

/***** Globals *****/

{WHITE_SPACE_CHAR}+     { break; }

{COMMENT}               { break; }

{QUOTED_STR}            { return yytext().substring(1, yylength() - 1); }

{UNQUOTED_STR}          { return yytext(); }


/* Catch-all: Print error message and try again. */
.                       { badChar(yycharat(0)); return yylex(); }

/***** End Globals *****/
