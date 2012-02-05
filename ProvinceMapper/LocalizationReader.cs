using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace ProvinceMapper
{
    class LocalizationReader
    {
        public LocalizationReader(string mapFolder, List<Province> provinceList, StatusUpdate su)
        {
            DirectoryInfo locFolder = new DirectoryInfo(Path.Combine(mapFolder, "..\\Localisation"));
            FileInfo[] locFiles = locFolder.GetFiles("*.csv");
            int fileIndex = 0;
            double fileFactor = 1.0 / locFiles.Count();
            foreach (FileInfo fi in locFiles)
            {
                StreamReader sr = new StreamReader(fi.FullName, Encoding.GetEncoding(1252));
                sr.ReadLine(); // discard first line
                while (!sr.EndOfStream)
                {
                    string locEntry = sr.ReadLine();
                    string[] locTokens = locEntry.Split(';');
                    if (locTokens[0].StartsWith("PROV"))
                    {
                        int provNum = 0;
                        if (int.TryParse(locTokens[0].Substring(4), out provNum))
                        {
                            foreach (Province p in provinceList)
                            {
                                if (p.ID == provNum)
                                {
                                    p.name = locTokens[1];
                                    break;
                                }
                            }
                        }
                    }

                    su(100.0 * fileFactor * (fileIndex + (double)sr.BaseStream.Position / sr.BaseStream.Length));
                }
                sr.Close();
                ++fileIndex;
            }
        }
    }
}
