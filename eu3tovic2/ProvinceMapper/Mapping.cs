using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ProvinceMapper
{
    class Mapping
    {
        public List<Province> srcProvs = new List<Province>();
        public List<Province> destProvs = new List<Province>();

        public Mapping()
        {
        }

        public Mapping(string line, string srcTag, string destTag, List<Province> possibleSources, List<Province> possibleDests)
        {
            string[] tokens = line.Split(' ', '\t');
            int parseMode = 0;
            foreach (string s in tokens)
            {
                if (s == "=" || s == String.Empty)
                {
                    continue;
                }
                else if (s == "link" || s == "{" || s == "}" || s == "=")
                {
                    parseMode = 0;
                }
                else if (s == srcTag)
                {
                    parseMode = 1;
                }
                else if (s == destTag)
                {
                    parseMode = 2;
                }
                else if (s[0] == '#')
                {
                    break;
                }
                else
                {
                    int provID = int.Parse(s.Trim('}'));
                    switch (parseMode)
                    {
                        case 1:
                            {
                                // provID is src
                                Province prov = possibleSources.Find(
                                    delegate(Province p)
                                    {
                                        return p.ID == provID;
                                    });
                                if (prov.mapping != null)
                                    throw new Exception(String.Format("Province \"{0}\" appears in more than one mapping!", prov.ToString()));
                                prov.mapping = this;
                                srcProvs.Add(prov);
                                break;
                            }
                        case 2:
                            {
                                // provID is dest
                                Province prov = possibleDests.Find(
                                    delegate(Province p)
                                    {
                                        return p.ID == provID;
                                    });
                                if (prov.mapping != null)
                                    throw new Exception(String.Format("Province \"{0}\" appears in more than one mapping!", prov.ToString()));
                                prov.mapping = this;
                                destProvs.Add(prov);
                                break;
                            }
                        default:
                            throw new Exception(String.Format("Unexpected token {0}", s));
                    }

                    parseMode = 0;
                }
            }
        }
        
        public override string ToString()
        {
            string comma = String.Empty;
            string retval = String.Empty;
            foreach (Province p in srcProvs)
            {
                retval += comma;
                retval += p.name;
                comma = ", ";
            }
            comma = String.Empty;
            retval += " -> ";
            foreach (Province p in destProvs)
            {
                retval += comma;
                retval += p.name;
                comma = ", ";
            }
            return retval;
        }

        public string ToOutputString(string srcTag, string destTag)
        {
            if (isIncomplete())
                return "";
            string retval = String.Empty;
            retval += "link = { ";
            foreach (Province p in srcProvs)
            {
                retval += srcTag + " = " + p.ID.ToString() + " ";
            }
            foreach (Province p in destProvs)
            {
                retval += destTag + " = " + p.ID.ToString() + " ";
            }
            retval += "}\t# ";
            retval += this.ToString();
            return retval;
        }

        public bool isManyToMany()
        {
            return (srcProvs.Count > 1) && (destProvs.Count > 1);
        }

        public bool isIncomplete()
        {
            return (srcProvs.Count == 0) || (destProvs.Count == 0);
        }

        public bool isInvalid()
        {
            return isManyToMany() || isIncomplete();
        }
    }
}
