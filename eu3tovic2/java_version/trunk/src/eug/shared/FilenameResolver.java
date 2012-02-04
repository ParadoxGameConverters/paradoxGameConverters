/*
 * FilenameResolver.java
 *
 * Created on February 9, 2007, 4:44 PM
 */

package eug.shared;

import eug.parser.EUGFileIO;
import java.io.File;
import java.util.List;

/**
 * Resolves filenames, given a main directory name and a mod name.
 * <p>
 * The default behavior is EU3-compatible: mod files are in a directory named
 * <code>getMainDirName() + getModPrefix() + getModName()</code>, and rules for
 * extending/replacing directories are in
 * <code>getMainDirName() + getModPrefix() + getModName() + ".mod"</code>.
 * <p>
 * To change to EU2-compatible behavior, use
 * {@link #setModFile(boolean) setModFile(false)} and
 * {@link #setModPrefix(String) setModPrefix("")}.
 * @author Michael Myers
 * @since EUGFile 1.02.01
 */
public final class FilenameResolver {
    
    private static final String FILE_SEPARATOR = System.getProperty("file.separator");
    
    private static final String defaultMainDir = "C:/Program Files/Paradox Interactive/Eu3 - DEMO";
    
    /** The base directory. */
    private String mainDirName;
    
    private String modName;
    
    private String modPrefix = "mod/";
    
    private String modDirName;
    
    private List<String> extended;
    private List<String> replaced;
    
    private boolean usingMod;
    
    /** true <=> main mod directory has a .mod file corresponding to each mod. */
    private boolean modFile = true;
    
    
    /**
     * Creates a new instance of FilenameResolver, loading configuration from
     * the given file.
     * @param configFile a file containing at least the following two entries:
     * <ul>
     * <li><code>maindir = "&lt;full path&gt;"</code></li>
     * <li><code>moddir = "&lt;name of mod&gt;"</code></li>
     * </ul>
     */
    public FilenameResolver(File configFile) {
        initDirNames(configFile);
    }
    
    /**
     * Creates a new instance of FilenameResolver with the given main directory
     * and no mod.
     * @param mainDirName the fully qualified name of the main directory.
     */
    public FilenameResolver(String mainDirName) {
        setMainDirectory(mainDirName);
        setModName("");
    }
    
    /**
     * Creates a new instance of FilenameResolver with the given main directory
     * and mod name.
     * @param mainDirName the fully qualified name of the main directory.
     * @param modName the unqualified name of the mod.
     */
    public FilenameResolver(String mainDirName, String modName) {
        setMainDirectory(mainDirName);
        setModName(modName != null ? modName : "");
    }
    
    private void initDirNames(File cfgFile) {
        GenericObject cfg = EUGFileIO.load(cfgFile);
        
        if (cfg == null) {
            setMainDirectory(defaultMainDir);
            setModName("");
        } else {
            setMainDirectory(cfg.getString("maindir"));
            setModName(cfg.getString("moddir"));
        }
    }
    
    /**
     * Sets the main directory name to the given string.
     * @param dirName the main directory.
     * @throws NullPointerException if <code>dirName</code> is <code>null</code>.
     */
    public void setMainDirectory(String dirName) {
        mainDirName = dirName;
        
        if (!(mainDirName.endsWith("/") || mainDirName.endsWith("\\"))) {
            mainDirName += FILE_SEPARATOR;
        }
    }
    
    /**
     * Sets the mod name to the given string.
     * @param modName the name of the mod.
     * @throws NullPointerException if <code>modName</code> is <code>null</code>.
     */
    public void setModName(String modName) {
        this.modName = modName;
        
        if (modName.length() != 0) {
            usingMod = true;
            modDirName = mainDirName + modPrefix + modName + "/";
            
            if (modFile) {
                GenericObject mod = EUGFileIO.load(mainDirName + modPrefix + modName + ".mod");
                if (mod == null) {
                    modFile = false;
                    extended = null;
                    replaced = null;
                } else {
                    extended = mod.getStrings("extend");
                    replaced = mod.getStrings("replace");
                }
            }
        } else {
            usingMod = false;
            modDirName = mainDirName;
        }
    }
    
    /**
     * Resolves the name of the given directory.
     * <p>
     * If no mod is being used, this simply returns the parameter. Otherwise,
     * the mod's .mod file will be checked to see if the directory is replaced.
     * If so, the mod path will be returned.
     *
     * @param dirName the name of the directory to resolve.
     * @return the path that the game will look in for the directory. This path
     * will end with the default file separator character.
     * @see resolveFilename(String)
     */
    public String resolveDirectory(String dirName) {
        if (dirName.startsWith("/") || dirName.startsWith("\\"))
            dirName = dirName.substring(1);
        if (!(dirName.endsWith("/") || dirName.endsWith("\\")))
            dirName += FILE_SEPARATOR;
        
        if (!usingMod)
            return mainDirName + dirName;
        
        String[] splitPath = splitParent(dirName);
        
        if (modFile) {
            if (replaced.contains(splitPath[0])) {
                return modDirName + dirName;
            } else if (extended.contains(splitPath[0])) {
                // XXX I don't think extending directories is done correctly
                if (new File(modDirName + dirName).exists()) {
                    return modDirName + dirName;
                } else {
                    return mainDirName + dirName;
                }
            } else {
                return mainDirName + dirName;
            }
        } else {
            if (new File(modDirName + dirName).exists())
                return modDirName + dirName;
            else
                return mainDirName + dirName;
        }
    }
    
    /**
     * Lists all files in the given directory. If a mod is being used and the
     * directory is set to extend, files in both the original and the mod
     * directory are returned.
     * @param dirName the name of the directory to list files in.
     */
    public File[] listFiles(String dirName) {
        if (dirName.startsWith("/") || dirName.startsWith("\\"))
            dirName = dirName.substring(1);
        if (!(dirName.endsWith("/") || dirName.endsWith("\\")))
            dirName += FILE_SEPARATOR;
        
        if (!usingMod)
            return new File(mainDirName + dirName).listFiles();
        
        String[] splitPath = splitParent(dirName);
        
        if (modFile) {
            if (replaced.contains(splitPath[0])) {
                return new File(modDirName + dirName).listFiles();
            } else if (extended.contains(splitPath[0])) {
                java.util.List<File> ret = new java.util.ArrayList<File>();
                
                for (File f : new File(mainDirName + dirName).listFiles()) {
                    ret.add(f);
                }
                
                File moddedDir = new File(modDirName + dirName);
                if (moddedDir.exists()) {
                    for (File f : moddedDir.listFiles()) {
                        ret.add(f);
                    }
                }
                
                return ret.toArray(new File[ret.size()]);
            } else {
                return new File(mainDirName + dirName).listFiles();
            }
        } else {
            if (new File(modDirName + dirName).exists())
                return new File(modDirName + dirName).listFiles();
            else
                return new File(mainDirName + dirName).listFiles();
        }
    }
    
    /**
     * Resolves the name of the given file.
     * <p>
     * If no mod is being used, this simply returns the parameter. Otherwise,
     * the mod's .mod file will be checked to see if the file's directory
     * (the part of the filename before the first '/' or '\' character) is
     * being replaced. If so, the mod path will be returned.
     *
     * @param filename the name of the file to resolve the path of.
     * @return the path that the game will look in for the file.
     * @see resolveDirectory(String)
     */
    public String resolveFilename(String filename) {
        if (filename.startsWith("/") || filename.startsWith("\\"))
            filename = filename.substring(1);
        
        if (!usingMod) {
            return mainDirName + filename;
        }
        
        String[] splitPath = splitParent(filename);
        
        if (modFile) {
            // EU3-style mod
            if (replaced.contains(splitPath[0])) {
                // Case 1: Directory is replaced.
                // Return the file in the moddir, even if it doesn't exist.
                return modDirName + filename;
            } else if (extended.contains(splitPath[0])) {
                // Case 2: Directory is extended.
                // Check if the file exists in the moddir.
                if (new File(modDirName + filename).exists()) {
                    // It does, so return it.
                    return modDirName + filename;
                } else {
                    // It doesn't, so return the file in the main dir.
                    return mainDirName + filename;
                }
            } else {
                // Case 3: Directory is not modded.
                // Return the file in the main dir.
                return mainDirName + filename;
            }
        } else {
            // EU2-style mod
            if (new File(modDirName + filename).exists())
                return modDirName + filename;
            else
                return mainDirName + filename;
        }
    }
    
    private static final String[] splitParent(String path) {
        int separatorIdx = path.indexOf('/');
        if (separatorIdx < 0) {
            separatorIdx = path.indexOf('\\');
            if (separatorIdx < 0)
                return new String[] {"", path};
        }
        
        return new String[] {
            path.substring(0, separatorIdx),
            path.substring(separatorIdx)
        };
    }
    
    // Inner classes to make the EU3 methods easier.
    // No longer used.
    
//    private static final class ProvFilter implements FilenameFilter {
//        private final String id;
//        public ProvFilter(int id) {
//            this.id = Integer.toString(id);
//        }
//        public boolean accept(File dir, String name) {
//            return name.startsWith(id) &&
//                    !name.endsWith("~") &&
//                    !Character.isDigit(name.charAt(id.length()));
//        }
//    }
//
//    private static final class TagFilter implements FilenameFilter {
//        private final String tag;
//        public TagFilter(String tag) {
//            this.tag = tag.toUpperCase();
//        }
//        public boolean accept(File dir, String name) {
//            return name.toUpperCase().startsWith(tag) && !name.endsWith("~");
//        }
//    }
    
    /**
     * EU3-specific method to find the province history file for the given
     * province.<p>
     * Originally package-private, but became public upon moving to eug.shared.
     * @return the name of the province history file, or <code>null</code> if
     * the file could not be found.
     */
    public String getProvinceHistoryFile(final int provId) {
        // Can't use resolveFilename, because we don't know what the file is
        // called.
        if (usingMod) {
            if (replaced.contains("history")) {
                // Case 1: History is replaced.
                // If there is a file in the mod's province history folder that
                // starts with the provId, return it.
                // Otherwise, return null.
                return getFile(modDirName + "history/provinces", Integer.toString(provId), true);
            } else if (extended.contains("history")) {
                // Case 2: History is extended.
                // If there is a file in the mod's province history folder that
                // starts with the provId, return it.
                // Otherwise, try to return the vanilla file.
                String filename = getFile(modDirName + "history/provinces", Integer.toString(provId), true);
                if (filename != null)
                    return filename;
                else
                    return getVanillaProvHistoryFile(provId);
            } else {
                // Case 3: History is not modded.
                // Try to return the vanilla file.
                return getVanillaProvHistoryFile(provId);
            }
        } else {
            // No mod. Try to return the vanilla file.
            return getVanillaProvHistoryFile(provId);
        }
    }
    
    private String getVanillaProvHistoryFile(final int provId) {
        return getFile(mainDirName + "history/provinces", Integer.toString(provId), true);
    }
    
    /**
     * EU3-specific method to find the country history file for the given
     * country.
     * @since EUGFile 1.04.00pre1
     */
    public String getCountryHistoryFile(final String tag) {
        if (usingMod) {
            if (replaced.contains("history")) {
                return getFile(modDirName + "history/countries", tag, false);
            } else if (extended.contains("history")) {
                String filename = getFile(modDirName + "history/countries", tag, false);
                if (filename != null)
                    return filename;
                else
                    return getVanillaCtryHistoryFile(tag);
            } else {
                return getVanillaCtryHistoryFile(tag);
            }
        } else {
            return getVanillaCtryHistoryFile(tag);
        }
    }
    
    private String getVanillaCtryHistoryFile(final String tag) {
        return getFile(mainDirName + "history/countries", tag, false);
    }
    
    private java.util.Map<String, String[]> directories =
            new java.util.HashMap<String, String[]>();
    
    private String getFile(final String dirname, final String start, final boolean exactMatch) {
        String[] array = directories.get(dirname);
        if (array == null) {
            array = new File(dirname).list();
            directories.put(dirname, array);
        }
        
        final int length = start.length();
        
        for (String name : array) {
            if (name.substring(0, length).equalsIgnoreCase(start) && //!name.contains("~") &&
                    (!exactMatch || !Character.isLetterOrDigit(name.charAt(length)))) {
                return new File(dirname + File.separatorChar + name).getAbsolutePath();
            }
        }
        return null;
    }
    
    /**
     * Directory information for province and country history files is cached,
     * to avoid calling <code>File.listFiles()</code> every time a file is
     * requested. This method clears the caches to force reloading of the
     * directory information.
     */
    public void reset() {
        directories.clear();
    }
    
    /**
     * Returns the prefix used between the main directory name and the mod name.
     * For EU3, this is "mod/".
     */
    public String getModPrefix() {
        return modPrefix;
    }
    
    /**
     * Sets the prefix used between the main directory name and the mod name.
     * For EU3, this is "mod/".
     * <p>
     * For EU2 compatibility, set this to "" and set modFile to
     * <code>false</code>.
     * @param prefix the prefix that all mod directories start with.
     * @see #setModFile
     */
    public void setModPrefix(String prefix) {
        modPrefix = prefix;
        setModName(modName);
    }
    
    /**
     * Returns the main directory name. This method is not generally useful;
     * {@link #getModDirName()} is more common.
     */
    public String getMainDirName() {
        return mainDirName;
    }
    
    /**
     * Returns the fully qualified name of the mod directory, ending with a
     * file separator. Use this method when creating a new file, for example.
     * @since EUGFile 1.04.00pre1
     */
    public String getModDirName() {
        return modDirName;
    }
    
    /**
     * Returns the name of the mod, or "" if no mod is being used.
     * @return the name of the mod being used.
     * @since EUGFile 1.07.00pre1
     */
    public String getModName() {
        return modName;
    }
    
    /**
     * @return true if an EU3-style .mod file is being used.
     */
    public boolean hasModFile() {
        return modFile;
    }
    
    public void setModFile(boolean modFile) {
        this.modFile = modFile;
        setModName(modName);
    }
    
    /**
     * @return <code>true</code> if the given directory is extended.
     * @since EUGFile 1.04.00pre1
     */
    public boolean isExtended(String directory) {
        return usingMod && extended.contains(directory);
    }
    
    /**
     * @return <code>true</code> if the given directory is replaced.
     * @since EUGFile 1.04.00pre1
     */
    public boolean isReplaced(String directory) {
        return usingMod && replaced.contains(directory);
    }
    
}
