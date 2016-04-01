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

            // first, check for the new YML localization (EUIV)
            FileInfo[] ymlFile = locFolder.GetFiles("prov_names_l_english.yml");
            if (ymlFile.Count() == 1)
            {
                ReadYML(ymlFile[0], provinceList, su);
                return;
            }

            // no relevant YML...read all the CSVs instead
            FileInfo[] locFiles = locFolder.GetFiles("*.csv");
            int fileIndex = 0;
            double fileFactor = 1.0 / locFiles.Count();
            foreach (FileInfo fi in locFiles)
            {
                ReadCSV(fi, provinceList, su, fileFactor, fileIndex);
                ++fileIndex;
            }
        }

        private void ReadYML(FileInfo File, List<Province> provinceList, StatusUpdate su)
        {
            StreamReader sr = new StreamReader(File.FullName, Encoding.UTF8);
            sr.ReadLine(); // discard first line
            while (!sr.EndOfStream)
            {
                string locEntry = sr.ReadLine();
                string[] locTokens = locEntry.Split(':');
                if (locTokens[0].StartsWith(" PROV"))
                {
                    int provNum = 0;
                    if (int.TryParse(locTokens[0].Substring(5), out provNum))
                    {
                        foreach (Province p in provinceList)
                        {
                            if (p.ID == provNum)
                            {
                                p.name = locTokens[1].Trim().Trim('\"');
                                break;
                            }
                        }
                    }
                }

                su(100.0 * ((double)sr.BaseStream.Position / sr.BaseStream.Length));
            }
            sr.Close();
        }

        private void ReadCSV(FileInfo File, List<Province> provinceList, StatusUpdate su, double fileFactor, int fileIndex)
        {
            StreamReader sr = new StreamReader(File.FullName, Encoding.GetEncoding(1252));
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
        }
    }
}
