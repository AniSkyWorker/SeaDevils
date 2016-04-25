#include "stdafx.h"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <iostream>
#include <fstream>

using namespace boost::numeric::ublas;

class CLandfillAggregator
{
public:
	CLandfillAggregator(unsigned & m_matrixRowsCount, unsigned & m_matrixColumnsCount)
		:m_matrixRowsCount(m_matrixRowsCount)
		,m_matrixColumnsCount(m_matrixColumnsCount)
		,m_landfill(m_matrixRowsCount + 2, m_matrixColumnsCount + 2)
	{
		for (unsigned i = 0; i < m_landfill.size1(); i++)
		{
			m_landfill(i, 0).first = -1;
			m_landfill(i, 0).second = -1;
		}
		for (unsigned i = 0; i < m_landfill.size1(); i++)
		{
			m_landfill(i, m_landfill.size2() - 1).first = -1;
			m_landfill(i, m_landfill.size2() - 1).second = -1;
		}
		for (unsigned j = 0; j < m_landfill.size2(); j++)
		{
			m_landfill(0, j).first = -1;
			m_landfill(0, j).second = -1;
		}
		for (unsigned j = 0; j < m_landfill.size2(); j++)
		{
			m_landfill(m_landfill.size1() - 1, j).first = -1;
			m_landfill(m_landfill.size1() - 1, j).second = -1;
		}
	}

	void RunDevilWave()
	{
		if (m_landfill(1, 1).first != -1)
		{
			if ((m_landfill(1, 1).second > 1 + m_landfill(1, 1).first) || (m_landfill(1, 1).second == m_landfill(1, 1).first))
			{
				m_landfill(1, 1).second = 1 + m_landfill(1, 1).first;
				SpreadDevilWave(2, 1, m_landfill(1, 1).second);
				SpreadDevilWave(1, 2, m_landfill(1, 1).second);
			}
		}
	}

	void InitFromFile(std::ifstream & in)
	{
		char buffer;
		for (unsigned i = 1; i < m_landfill.size1() - 1; i++)
		{
			for (unsigned j = 1; j < m_landfill.size2() - 1; j++)
			{
				in >> buffer;
				while (buffer == '\n')
				{
					in >> buffer;
				}
				if (buffer == '.')
				{
					m_landfill(i, j).first = 1;
					m_landfill(i, j).second = 1;
				}
				if (buffer == '@')
				{
					m_landfill(i, j).first = m_matrixRowsCount*m_matrixColumnsCount;
					m_landfill(i, j).second = m_matrixRowsCount*m_matrixColumnsCount;
				}
			}
		}
	}

	std::string GetMinPathCounts()
	{
		int fields = m_landfill(m_matrixRowsCount, m_matrixColumnsCount).second - 1;
		int countOfWaterFields = fields / (m_matrixRowsCount*m_matrixColumnsCount);
		return std::to_string(countOfWaterFields) + ' ' + std::to_string(fields - (countOfWaterFields * m_matrixRowsCount*m_matrixColumnsCount));
	}

private:
	void SpreadDevilWave(unsigned i, unsigned j, int currentWeight)
	{
		if (m_landfill(i, j).first != -1)
		{
			if ((m_landfill(i, j).second > currentWeight + m_landfill(i, j).first) || (m_landfill(i, j).second == m_landfill(i, j).first))
			{
				m_landfill(i, j).second = currentWeight + m_landfill(i, j).first;
				SpreadDevilWave(i + 1, j, m_landfill(i, j).second);
				SpreadDevilWave(i, j + 1, m_landfill(i, j).second);
				SpreadDevilWave(i - 1, j, m_landfill(i, j).second);
				SpreadDevilWave(i, j - 1, m_landfill(i, j).second);
			}
		}
	}

	unsigned m_matrixRowsCount;
	unsigned m_matrixColumnsCount;
	matrix<std::pair<int, int>> m_landfill;
};

int main()
{
	std::ifstream in("input.txt");

	unsigned M, N;
	in >> M >> N;

	CLandfillAggregator m_landfill(M, N);
	m_landfill.InitFromFile(in);
	m_landfill.RunDevilWave();

	std::ofstream out("output.txt");
	out << m_landfill.GetMinPathCounts() << std::endl;

	return 0;
}