#pragma once
#include "stdafx.h"
using namespace boost::numeric::ublas;

struct SVisualElements : public sf::Drawable
{
	SVisualElements(int i, int j) :texts(i, j), rects(i, j) {};
	matrix<sf::Text> texts;
	matrix<sf::RectangleShape> rects;

	void draw(sf::RenderTarget &target, sf::RenderStates states) const override
	{
		for (size_t i = 0; i < rects.size1(); i++)
		{
			for (size_t j = 0; j < rects.size2(); j++)
			{
				target.draw(rects(i,j), states);
				target.draw(texts(i,j), states);
			}
		}
	}
};

class CLandfillAggregator
{
public:
	CLandfillAggregator(unsigned & m_matrixRowsCount, unsigned & m_matrixColumnsCount);

	void InitFromFile(std::ifstream & in);

	void RunDevilWave();
	bool SpreadDevilWave();
	
	int GetWaveLevel(int i, int j) const;
	std::string GetMinPathCounts() const;
	std::shared_ptr<SVisualElements> GetPtrToVisual() const;
	
private:
	void InitBorders();
	void AddVisual();

	unsigned m_matrixRowsCount;
	unsigned m_matrixColumnsCount;
	matrix<std::pair<int, int>> m_landfill;
	std::queue<std::pair<std::pair<int, int>, int>> m_waveQueue;
	std::shared_ptr<SVisualElements> m_visual;
	std::pair<int, int> m_selected;
};
