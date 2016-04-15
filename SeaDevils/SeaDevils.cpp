#include "stdafx.h"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <iostream>
#include <fstream>

using namespace boost::numeric::ublas;

class CButts
{
public:
	CButts(unsigned & M, unsigned & N)
		:M(M)
		,N(N)
		,butts(M + 2, N + 2)
	{
		for (unsigned i = 0; i < butts.size1(); i++)
		{
			butts(i, 0).first = -1;
			butts(i, 0).second = -1;
		}
		for (unsigned i = 0; i < butts.size1(); i++)
		{
			butts(i, butts.size2() - 1).first = -1;
			butts(i, butts.size2() - 1).second = -1;
		}
		for (unsigned j = 0; j < butts.size2(); j++)
		{
			butts(0, j).first = -1;
			butts(0, j).second = -1;
		}
		for (unsigned j = 0; j < butts.size2(); j++)
		{
			butts(butts.size1() - 1, j).first = -1;
			butts(butts.size1() - 1, j).second = -1;
		}
	}

	void DevilWave(unsigned i = 1, unsigned j = 1, int currentWeight = 1)
	{
		if (butts(i, j).first != -1)
		{
			if ((butts(i, j).second > currentWeight + butts(i, j).first) || (butts(i, j).second == butts(i, j).first))
			{
				butts(i, j).second = currentWeight + butts(i, j).first;
				DevilWave(i + 1, j, butts(i, j).second);
				DevilWave(i, j + 1, butts(i, j).second);
				DevilWave(i - 1, j, butts(i, j).second);
				DevilWave(i, j - 1, butts(i, j).second);
			}
		}
	}

	void InitFromFile(std::ifstream & in)
	{
		char buffer;
		for (unsigned i = 1; i < butts.size1() - 1; i++)
		{
			for (unsigned j = 1; j < butts.size2() - 1; j++)
			{
				in >> buffer;
				while (buffer == '\n')
				{
					in >> buffer;
				}
				if (buffer == '.')
				{
					butts(i, j).first = 1;
					butts(i, j).second = 1;
				}
				if (buffer == '@')
				{
					butts(i, j).first = M*N;
					butts(i, j).second = M*N;
				}
			}
		}
	}

	std::string GetMinPathCounts()
	{
		int fields = butts(M, N).second - 1;
		int countOfWaterFields = fields / (M*N);
		return std::to_string(countOfWaterFields) + ' ' + std::to_string(fields - (countOfWaterFields * M*N));
	}

private:
	unsigned M;
	unsigned N;
	matrix<std::pair<int, int>> butts;
};

int main()
{
	std::ifstream in("input.txt");
	unsigned M, N;
	in >> M >> N;
	CButts butts(M, N);
	butts.InitFromFile(in);
	butts.DevilWave();
	std::ofstream out("output.txt");
	out << butts.GetMinPathCounts() << std::endl;
    return 0;
}