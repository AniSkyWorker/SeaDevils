#include "stdafx.h"
#include "LandfillAggregator.h"

CLandfillAggregator::CLandfillAggregator(unsigned & m_matrixRowsCount, unsigned & m_matrixColumnsCount)
	:m_matrixRowsCount(m_matrixRowsCount)
	, m_matrixColumnsCount(m_matrixColumnsCount)
	, m_landfill(m_matrixRowsCount + 2, m_matrixColumnsCount + 2)
{
	InitBorders();
	AddVisual();
}

int CLandfillAggregator::GetWaveLevel(int i, int j) const
{
	return m_landfill(i, j).second;
}

void CLandfillAggregator::RunDevilWave()
{
	if (m_landfill(1, 1).first != -1)
	{
		m_landfill(1, 1).first = -1;
		m_waveQueue.push(std::make_pair(std::make_pair(2, 1), m_landfill(1, 1).second));
		m_waveQueue.push(std::make_pair(std::make_pair(1, 2), m_landfill(1, 1).second));
	}
}

void CLandfillAggregator::InitFromFile(std::ifstream & in)
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

void CLandfillAggregator::InitBorders()
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

void CLandfillAggregator::AddVisual()
{
	m_visual = std::make_shared<SVisualElements>(m_landfill.size1(), m_landfill.size2());
}

std::string CLandfillAggregator::GetMinPathCounts() const
{
	int fields = m_landfill(m_matrixRowsCount, m_matrixColumnsCount).second;
	int countOfWaterFields = fields / (m_matrixRowsCount*m_matrixColumnsCount);
	return std::to_string(countOfWaterFields) + ' ' + std::to_string(fields - (countOfWaterFields * m_matrixRowsCount*m_matrixColumnsCount));
}

std::shared_ptr<SVisualElements> CLandfillAggregator::GetPtrToVisual() const
{
	return m_visual;
}

bool CLandfillAggregator::SpreadDevilWave()
{
	if (!m_waveQueue.empty())
	{
		auto queue = m_waveQueue.front();
		m_waveQueue.pop();
		auto current = m_landfill(queue.first.first, queue.first.second);
		if (current.first != -1)
		{
			m_visual->rects(m_selected.first, m_selected.second).setFillColor(m_landfill(m_selected.first, m_selected.second).first == -1
				? sf::Color::Red : m_landfill(m_selected.first, m_selected.second).first == 1
				? sf::Color::Green : sf::Color::Blue);
			if (current.second > (queue.second + current.first) || current.second == current.first)
			{
				current.second = queue.second + current.first;
				m_waveQueue.push({ { queue.first.first + 1, queue.first.second }, current.second });
				m_waveQueue.push({ { queue.first.first, queue.first.second + 1 }, current.second });
				m_waveQueue.push({ { queue.first.first - 1, queue.first.second }, current.second });
				m_waveQueue.push({ { queue.first.first, queue.first.second - 1 }, current.second });
				m_landfill(queue.first.first, queue.first.second) = current;
				m_visual->texts(queue.first.first, queue.first.second).setString(std::to_string(current.second));
				m_visual->rects(queue.first.first, queue.first.second).setFillColor(sf::Color::Yellow);
				m_selected = queue.first;
			}
		}
		return true;
	}
	return false;
}