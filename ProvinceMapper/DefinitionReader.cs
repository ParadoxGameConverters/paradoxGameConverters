using System.Collections.Generic;
using System.Text;
using System.IO;



namespace ProvinceMapper
{
	class DefinitionReader
	{
		public DefinitionReader(string path, StatusUpdate progress)
		{
			StreamReader reader = new StreamReader(path, Encoding.GetEncoding(1252));
			reader.ReadLine(); // discard first line

			while (!reader.EndOfStream)
			{
				string province = reader.ReadLine();
				string[] provinceTokens = province.Split(';');
				if (IsRNWProvince(provinceTokens) || IsUnusedProvince(provinceTokens))
				{
					continue;
				}

				try
				{
					Province p = new Province(provinceTokens);
					provinces.Add(p);
				}
				catch
				{
				}

				progress(100.0 * reader.BaseStream.Position / reader.BaseStream.Length);
			}

			reader.Close();
		}

		public List<Province> provinces = new List<Province>();

		private bool IsRNWProvince(string[] provinceTokens)
		{
			return (provinceTokens[4] == "RNW");
		}

		private bool IsUnusedProvince(string[] provinceTokens)
		{
			return ((provinceTokens[4].Length > 5) && (provinceTokens[4].Substring(0, 6) == "Unused"));
		}
	}
}
