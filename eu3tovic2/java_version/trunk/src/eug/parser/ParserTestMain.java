package eug.parser;

import eug.shared.GenericList;
import eug.shared.GenericObject;
import eug.shared.ObjectVariable;
import eug.shared.Style;
import eug.shared.WritableObject;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;

/**
 * Simple tester for the parser.
 * @author Michael Myers
 */
public final class ParserTestMain {
    
    private ParserTestMain() { }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        final GenericObject obj = EUGFileIO.load(
                (args[0]
//                Utilities.findNewestFile(
//                    new java.io.File(
//                        "C:/Program Files/Strategy First/Europa Universalis 2/AGCEEP/Scenarios/Save Games"
//                    )
                )
                );
        
//        for (GenericObject country : obj.getChildren("country")) {
////            for (GenericObject child)
//            if (country.getString("tag").equals("POR")) {
//                search(country, "region");
////                System.out.println(country.getChild("ai"));
//            }
//        }
        
        final long start = System.nanoTime();
        try {
            BufferedWriter bw = new BufferedWriter(new FileWriter(args[1]));
            obj.toFileString(bw, Style.DEFAULT);
        } catch (FileNotFoundException ex) {
            ex.printStackTrace();
            System.exit(1);
        } catch (IOException ex) {
            ex.printStackTrace();
            System.exit(1);
        }
        System.out.println("Saving took " + (System.nanoTime() - start) + " ns.");
    }
    
//    private static void search(WritableObject obj, String name) {
//        if (obj instanceof GenericObject)
//            search((GenericObject)obj, name);
//        else if (obj instanceof GenericList)
//            search((GenericList)obj, name);
//        else if (obj instanceof ObjectVariable)
//            search((ObjectVariable)obj, name);
//        else
//            System.err.println("Unknown type: " + obj.getClass());
//    }
//    
//    private static void search(GenericObject obj, String name) {
//        if (obj.name.equals(name)) {
//            System.out.print(">>>>>Found one in " + obj.getParent().name + " in " + obj.getParent().getParent().name);
//            if (obj.getParent().getParent().name.equals("country"))
//                System.out.println(" (tag = " + obj.getParent().getParent().getString("tag") + ")");
//            else
//                System.out.println();
//        }
//        for (WritableObject child : obj.getAllWritable())
//            search(child, name);
//    }
//    
//    private static void search(GenericList list, String name) {
////        System.out.println("Checking a list...");
//        if (list.getName().equals(name))
//            System.out.println(">>>>>Found one (list)");
////        else
////            System.out.println("Not found in " + list.getName());
//    }
//    
//    private static void search(ObjectVariable var, String name) {
//        if (var.varname.equals(name))
//            System.out.println(">>>>>Found one (variable)");
//    }
}

