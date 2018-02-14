/*Copyright (c) 2018 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#ifndef ADJACENCY_MAPPER_H
#define ADJACENCY_MAPPER_H



#include <fstream>
#include <map>
#include <optional>
#include <string>
#include <vector>



namespace mappers
{
	class adjacencyMapper
	{
		public:
			static std::optional<std::vector<int>> getVic2Adjacencies(int Vic2Province)
			{
				return getInstance()->GetVic2Adjacencies(Vic2Province);
			}

		private:
			static adjacencyMapper* instance;
			static adjacencyMapper* getInstance()
			{
				if (instance == nullptr)
				{
					instance = new adjacencyMapper;
				}

				return instance;
			}

			adjacencyMapper();
			std::string getAdjacencyFilename();
			void inputAdjacencies(std::istream& adjacenciesFile);
			std::vector<int> readAnAdjacenciesSet(std::istream& adjacenciesFile, unsigned int numAdjacencies);

			void outputAdjacenciesMapData();

			std::optional<std::vector<int>> GetVic2Adjacencies(int Vic2Province);


			std::map<int, std::vector<int>> adjacencyMap;
	};
}







#endif // ADJACENCY_MAPPER_H