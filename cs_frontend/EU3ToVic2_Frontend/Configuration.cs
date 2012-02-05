using System;
using System.Collections.Generic;
using System.Text;
using System.Xml.Serialization;
using System.IO;

namespace EU3ToVic2_Frontend
{
    public class Configuration
    {
        public string V2Path;
        public DateTime StartDate;
        public enum EU3Type
        {
            Auto,
            HeirToTheThrone,
            DivineWind
        }
        public EU3Type GameType;

        public void WriteConfig(string path)
        {
            TextWriter tw = new StreamWriter(path);
            tw.WriteLine("configuration = {");
            tw.WriteLine("\tv2directory = \"" + V2Path + "\"");
            tw.WriteLine("\tstart_date = " + StartDate.ToString("yyyy.MM.dd"));
            switch (GameType)
            {
                case EU3Type.Auto:
                    tw.WriteLine("\tgametype = auto");
                    break;
                case EU3Type.DivineWind:
                    tw.WriteLine("\tgametype = dw");
                    break;
                case EU3Type.HeirToTheThrone:
                    tw.WriteLine("\tgametype = httt");
                    break;
            }
            tw.WriteLine("}");
            tw.Close();
        }

        public void WriteXML(string path)
        {
            XmlSerializer ser = new XmlSerializer(typeof(Configuration));
            TextWriter tw = new StreamWriter(path);
            ser.Serialize(tw, this);
            tw.Close();
        }

        public static Configuration ReadXML(string path)
        {
            XmlSerializer deser = new XmlSerializer(typeof(Configuration));
            TextReader tr = new StreamReader(path);
            Object result = deser.Deserialize(tr);
            tr.Close();
            return result as Configuration;
        }
    }
}
