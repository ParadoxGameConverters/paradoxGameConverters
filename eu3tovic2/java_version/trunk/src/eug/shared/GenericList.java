
package eug.shared;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.Serializable;
import java.util.Comparator;
import java.util.Iterator;
import java.util.List;
import java.util.Vector;

/**
 * This class represents a list from the saved game. For example:<br>
 * <code>ownedprovinces = { 45 46 55}</code>
 * <p>
 * Since this class implements <code>Iterable&lt;String&gt;</code>, it can be used with a
 * for-each loop:<br>
 * <pre>
 *       GenericObject country = ...;
 *       for (String id : country.getList("ownedprovinces")) {
 *           // do something
 *       }
 * </pre>
 */
public final class GenericList implements WritableObject, Iterable<String>, Cloneable {
    
    private HeaderComment headComment = null;
    private InlineComment inlineComment = null;
    
    //modif leo - attributs en public
    private Vector<String> list;
    private String varname;
    private boolean quotes = false;
    
    /**
     * Package-private constructor. The only way to access this class is
     * through {@link GenericObject#addList(String, String[])}, {@link
     * GenericObject#addList(String, java.util.List)}, or {@link
     * GenericObject#createList(String)}.
     */
    GenericList(String name) {
        varname = name;
        list = new Vector<String>();
    }
    
    public void setHeaderComment(String comment) {
        if (comment == null || comment.length() == 0)
            headComment = null;
        else
            headComment = new HeaderComment(comment);
    }
    
    public String getHeaderComment() {
        return (headComment == null ? "" : headComment.getComment());
    }
    
    public void setInlineComment(String comment) {
        if (comment == null || comment.length() == 0)
            inlineComment = null;
        else
            inlineComment = new InlineComment(comment);
    }
    
    public String getInlineComment() {
        return (inlineComment == null ? "" : inlineComment.getComment());
    }
    
    public String get(int id) {
        if (id < list.size())
            return list.get(id);
        else
            return null;
    }
    
    /**
     * @return the list (modifying the returned value will have no effect on
     * this <code>GenericList</code>).
     * @since EUGFile 1.06.00pre3
     */
    public List<String> getList() {
        return (List<String>) list.clone();
    }
    
    public int size() {
        return list.size();
    }
    
    public int indexOf(String val) {
        return list.indexOf(val);
    }
    
    public void delete(int id) {
        list.remove(id);
    }
    
    public boolean delete(String val) {
        // The List<E> interface guarantees that it remains unchanged if the
        // element is not present in the list, so no check is necessary.
        return list.remove(val);
    }
    
    public void clear() {
        list.clear();
    }
    
    public void add(final String val) {
        add(val, true);
    }
    
    public void add(final String val, boolean forceQuotes) {
        if (!quotes && (forceQuotes || val.contains(" ") || val.contains("\"")))
            quotes = true;
        list.add(trimQuotes(val));
    }
    
    public void add(int val) {
        list.add(Integer.toString(val));
    }
    
    public void add(double val) {
        list.add(Double.toString(val));
    }
    
    public void insert(final String val, boolean forceQuotes, int idx) {
        list.add(idx, trimQuotes(val));
        if (!quotes && (forceQuotes || val.contains(" ") || val.contains("\"")))
            quotes = true;
    }
    
    public void insert(int val, int idx) {
        list.add(idx, Integer.toString(val));
    }
    
    public void insert(double val, int idx) {
        list.add(idx, Double.toString(val));
    }
    
    public void replace(String oldVal, String newVal) {
        for (int i = 0; i < list.size(); i++) {
            if (list.get(0).equals(oldVal)) {
                list.set(i, newVal);
                break;
            }
        }
    }
    
    public void replaceAll(String oldVal, String newVal) {
        for (int i = 0; i < list.size(); i++)
            if (list.get(0).equals(oldVal))
                list.set(i, newVal);
    }
    
    private static final String trimQuotes(String s) {
        return s.replace('\"', ' ').trim();
    }
    
    public void addAll(String[] vals) {
        addAll(vals, false);
    }
    
    public void addAll(String[] vals, boolean sort) {
        for (String s : vals) {
            add(s, s.contains("\""));
        }
        
        if (sort)
            sort();
    }
    
    public void addAll(List<String> vals) {
        for (String s : vals)
            add(s, false);
    }
    
    public void addAll(GenericList other) {
        list.addAll(other.list);
        quotes = (quotes || other.quotes);
    }
    
    public void sort() {
        if (list.isEmpty())
            return;
        
        try {
            Double.parseDouble(list.get(0));
            java.util.Collections.sort(list, new NumberStringComparator());
        } catch (NumberFormatException ex) {
            java.util.Collections.sort(list);
        }
    }
    
    /** @since EUGFile 1.05.00pre4 */
    public void sort(Comparator<String> comparator) {
        if (list.isEmpty() || comparator == null)
            return;
        java.util.Collections.sort(list, comparator);
    }
    
    public boolean contains(String val) {
        return list.contains(val);
    }
    
    public void toFileString(final BufferedWriter bw, int depth) throws IOException {
        toFileString(bw, depth, Style.DEFAULT);
    }
    
    public void toFileString(final BufferedWriter bw, int depth, Style style) throws IOException {
        
        if (headComment != null)
            headComment.toFileString(bw, depth, style);
        
        style.printTab(bw, depth);
        
        bw.write(varname);
        style.printEqualsSign(bw, depth);
        bw.write("{ ");
        
        // only check for quotes once
        if (quotes) {
            for (String str : list) {
                bw.write("\"");
                bw.write(str);
                bw.write("\" ");
            }
        } else {
            for (String str : list) {
                bw.write(str);
                bw.write(" ");
            }
        }
        
        bw.write("}");
        
        if (inlineComment != null)
            inlineComment.toFileString(bw, depth, style);
    }
    
    @Override
    public String toString() {
        final java.io.StringWriter sw = new java.io.StringWriter();
        final BufferedWriter bw = new BufferedWriter(sw);
        
        try {
            this.toFileString(bw,0);
            bw.flush();
        } catch (IOException ex) {  // won't happen
            ex.printStackTrace();
        }
        
        return sw.toString();
    }
    
    public Iterator<String> iterator() {
        return list.iterator();
    }
    
    // Implement Serializable so that any list that uses this comparator can be
    // serialized.
    private static final class NumberStringComparator implements Comparator<String>, Serializable {
        private static final long serialVersionUID = 1L;
        public final int compare(String o1, String o2) {
            return Double.valueOf(o1).compareTo(Double.valueOf(o2));
        }
    }
    
    public String getName() {
        return varname;
    }
    
    public void setName(String name) {
        varname = name;
    }
    
    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        else if (!(obj instanceof GenericList))
            return false;
        
        GenericList gl = (GenericList) obj;
        
        return (quotes == gl.quotes) && varname.equals(gl.varname) && list.equals(gl.list);
    }

    @Override
    public int hashCode() {
        int hash = 7;
        hash = 17 * hash + (this.list != null ? this.list.hashCode() : 0);
        hash = 17 * hash + (this.varname != null ? this.varname.hashCode() : 0);
        hash = 17 * hash + (this.quotes ? 1 : 0);
        return hash;
    }
    
    @Override
    public GenericList clone() {
        final GenericList retValue = new GenericList(varname);
        
        if (headComment != null)
            retValue.setHeaderComment(headComment.getComment());
        if (inlineComment != null)
            retValue.setInlineComment(inlineComment.getComment());
        
        retValue.addAll(this);
        retValue.quotes = quotes;
        
        return retValue;
    }

    /** @since EUGFile 1.06.00pre3 */
    public void set(int index, String element) {
        list.set(index, element);
    }
}
