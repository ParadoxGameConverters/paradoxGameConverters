package eug.parser;


/**
 * Enumeration of all possible token types.
 */
public enum TokenType {
    
    /** Constant indicating that the end of the stream has been read. */
    EOF,
    
    /** Constant indicating that an unquoted string has been read. */
    ULSTRING,
    
    /** Constant indicating that a quoted string has been read. */
    DLSTRING,
    
    /** Constant indicating that an equals sign has been read. */
    EQUALS,
    
    /** Constant indicating that a left brace has been read. */
    LBRACE,
    
    /** Constant indicating that a right brace has been read. */
    RBRACE,
    
    /** Constant indicating that a comment has been read. */
    COMMENT,
    
    /** Constant indicating that an identifier ("xxx =") has been read. */
    IDENT,
    
    /** Constant indicating that a list has been read. */
    LIST,
    
    /** Constant indicating that a newline character (\r, \n, or \r\n) has been read. */
    NEWLINE,
}