/*
 * ParserException.java
 *
 * Created on February 17, 2007, 11:19 AM
 */

package eug.parser;

/**
 * Exception thrown when the parser encounters an illegal token.
 * <p>
 * For example, an instance of this class might be thrown if a warning is
 * produced while the parser is in strict mode.
 * @author Michael Myers
 * @since EUGFile 1.02.00
 */
public class ParserException extends RuntimeException {
    
    private static final long serialVersionUID = 1L;
    
    /**
     * Creates a new instance of <code>ParserException</code> without detail message.
     */
    public ParserException() {
    }
    
    /**
     * Constructs an instance of <code>ParserException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public ParserException(String msg) {
        super(msg);
    }
}
