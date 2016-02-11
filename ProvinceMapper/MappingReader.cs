using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace ProvinceMapper
{
    class MappingReader
    {
        public Dictionary<string, List<Mapping>> mappings = new Dictionary<string, List<Mapping>>();

        public string srcTag;
        public string destTag;

        private string path;

        public MappingReader(string _path, string _srcTag, string _destTag)
        {
            srcTag = _srcTag;
            destTag = _destTag;
            path = _path;
        }

        public MappingReader(string _path, string _srcTag, string _destTag, List<Province> srcProvs, List<Province> destProvs, StatusUpdate su)
        {
            srcTag  = _srcTag;
            destTag = _destTag;
            path    = _path;

            List<Mapping>   currentMappings = new List<Mapping>();
            string          mappingName     = "";
            int             level           = 0;

            StreamReader sr = new StreamReader(path, Encoding.GetEncoding(1252));
            while (!sr.EndOfStream)
            {
                string line = sr.ReadLine().Trim();
                if (line.Length > 0)
                {
                    if ((level == 0) && (!line.StartsWith("#")))
                    {
                        if (mappingName != "")
                        {
                            mappings.Add(mappingName, currentMappings);
                            currentMappings = new List<Mapping>();
                        }
                        string[] strings = line.Split('=');
                        mappingName      = strings[0];
                        mappingName.Trim();

                        level += line.Count(x => x == '{') - line.Count(x => x == '}');
                        continue;
                    }
                    if (line.StartsWith("link"))
                    {
                        try
                        {
                            currentMappings.Add(new ProvinceMapping(line, srcTag, destTag, srcProvs, destProvs, mappingName));
                        }
                        catch (Exception e)
                        {
                            System.Windows.Forms.MessageBox.Show(e.Message, "Error in mapping file");
                        }
                    }
                    else if (line.StartsWith("#"))
                    {
                        try
                        {
                            currentMappings.Add(new CommentMapping(line));
                        }
                        catch (Exception e)
                        {
                            System.Windows.Forms.MessageBox.Show(e.Message, "Error in mapping file");
                        }
                    }
                    else if (line.StartsWith("mappings") || line.StartsWith("{") || line.StartsWith("}"))
                    {
                        // ignore these lines
                    }
                    else
                    {
                        System.Windows.Forms.MessageBox.Show(String.Format("Error parsing province mapping file: line beginning '{0}' appears to be invalid.", line.Split(' ')[0]));
                    }

                    level += line.Count(x => x == '{') - line.Count(x => x == '}');
                }
                su(100.0 * sr.BaseStream.Position / sr.BaseStream.Length);
            }
            mappings.Add(mappingName, currentMappings);
            sr.Close();
        }

        public void Write()
        {
            Write(path);
        }

        public void Write(string path)
        {
            StreamWriter sw = new StreamWriter(path, false, Encoding.GetEncoding(1252));
            foreach (KeyValuePair<string, List<Mapping>> oneMapping in mappings)
            {
                sw.WriteLine("%s = {", oneMapping.Key);
                foreach (Mapping m in oneMapping.Value)
                {
                    sw.WriteLine(m.ToOutputString(srcTag, destTag));
                }
                sw.WriteLine("}");
            }
            sw.Close();
        }
    }
}
