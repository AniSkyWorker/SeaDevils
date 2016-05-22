#include "stdafx.h"
#include "AppMenu.h"
#include "tinyfiledialogs.h"

using namespace boost::numeric::ublas;
namespace
{
	const unsigned WINDOW_WIDTH = 800;
	const unsigned WINDOW_HEIGHT = 600;
	const unsigned MENU_BAR_HEIGHT = 20;
	const unsigned WINDOW_STYLE = sf::Style::Titlebar | sf::Style::Close;
	const int FRAME_SWITCH_INTERVAL = 1000;
}
class CLandfillAggregator
{
public:
	CLandfillAggregator(unsigned & m_matrixRowsCount, unsigned & m_matrixColumnsCount)
		:m_matrixRowsCount(m_matrixRowsCount)
		,m_matrixColumnsCount(m_matrixColumnsCount)
		,m_landfill(m_matrixRowsCount + 2, m_matrixColumnsCount + 2)
		, rects(m_landfill.size1(), m_landfill.size2())
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

		m_menu = std::make_unique<CAppMenu>("File");
		m_menu->SetFrame(sf::FloatRect(0, 0, float(WINDOW_WIDTH), float(MENU_BAR_HEIGHT)));
		m_menu->AddAction("Open...", std::bind(&CLandfillAggregator::AskOpenInput, this));
		m_menu->AddAction("Save...", std::bind(&CLandfillAggregator::AskSaveOutput, this));
		m_menu->SetActionEnabled(0, true);
	}

	void Draw()
	{
		sf::RenderWindow window;
		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;

		
		matrix<sf::Text> texts(m_landfill.size1(), m_landfill.size2());
		window.create(sf::VideoMode(640, 640), "Shape Viewer", sf::Style::Default, settings);
		window.setFramerateLimit(1);
		RunDevilWave();
		for (unsigned i = 0; i < rects.size1(); i++)
		{
			for (unsigned j = 0; j < rects.size2(); j++)
			{
				sf::RectangleShape rect(sf::Vector2f(window.getSize().x / rects.size1(), window.getSize().y / rects.size2()));
				rect.setOutlineThickness(1.f);
				rect.setOutlineColor(sf::Color::Black);
				rect.setFillColor(m_landfill(i, j).first == -1 ? sf::Color::Red : m_landfill(i, j).first == 1 ? sf::Color::Green : sf::Color::Blue);
				rect.setPosition(sf::Vector2f(i * window.getSize().x / rects.size1() + 1, j *window.getSize().y / rects.size2() + 1));
				rects(i, j) = rect;

			}

		}
		sf::Font font;
		font.loadFromFile("Sansation.ttf");
		for (unsigned i = 0; i < rects.size1(); i++)
		{
			for (unsigned j = 0; j < rects.size2(); j++)
			{
				sf::Text text(std::to_string(m_landfill(i, j).second), font, window.getSize().x / rects.size1() / 2);
				text.setPosition(rects(i, j).getPosition());
				text.setColor(sf::Color::Black);
				texts(i, j) = text;
			}
		}
		while (window.isOpen())
		{

			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					window.close();
				}
				m_menu->OnEvent(event);
			}

			window.clear(sf::Color::White);
			/*if (!m_waveQueue.empty())
			{
				SpreadDevilWave();
				for (unsigned i = 0; i < rects.size1(); i++)
				{
					for (unsigned j = 0; j < rects.size2(); j++)
					{
						texts(i, j).setString(std::to_string(m_landfill(i, j).second));
						window.draw(rects(i, j));
						window.draw(texts(i, j));
					}
				}
			}
			else
			{
				std::ofstream out("output.txt");
				out << GetMinPathCounts() << std::endl;
			}*/
			window.draw(*m_menu);
			window.display();

		}

	}
	void RunDevilWave()
	{
		if (m_landfill(1, 1).first != -1)
		{
			if ((m_landfill(1, 1).second > 1 + m_landfill(1, 1).first) || (m_landfill(1, 1).second == m_landfill(1, 1).first))
			{
				m_landfill(1, 1).second = 1 + m_landfill(1, 1).first;
				m_waveQueue.push(std::make_pair(std::make_pair(2, 1), m_landfill(1, 1).second));
				m_waveQueue.push(std::make_pair(std::make_pair(1, 2), m_landfill(1, 1).second));
			}
		}
		SpreadDevilWave();
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
	void SpreadDevilWave()
	{
		if(!m_waveQueue.empty())
		{
			auto queue = m_waveQueue.front();
			m_waveQueue.pop();
			auto current = m_landfill(queue.first.first, queue.first.second);
			if (current.first != -1)
			{
				if (current.second > (queue.second + current.first) || current.second == current.first)
				{
					current.second = queue.second + current.first;
					m_waveQueue.push({ {queue.first.first + 1, queue.first.second}, current.second });
					m_waveQueue.push({ {queue.first.first, queue.first.second + 1}, current.second });
					m_waveQueue.push({ {queue.first.first - 1, queue.first.second}, current.second });
					m_waveQueue.push({ {queue.first.first, queue.first.second - 1}, current.second });
					m_landfill(queue.first.first, queue.first.second) = current;
				}
			}
		}
	}

	void AskOpenInput()
	{
		const char *filters[] = { "*.txt" };
		char const *result = tinyfd_openFileDialog("Select input file", "", 1, filters, "", false);
		// Пользователь отменил выбор файла.
		if (result == nullptr)
		{
			return;
		}

		std::ifstream in(result);
		InitFromFile(in);
	}

	void AskSaveOutput()
	{
		const char *filters[] = { "*.txt" };
		char const *result = tinyfd_saveFileDialog("Select output file", "", 1, filters, "");
		// Пользователь отменил выбор файла.
		if (result == nullptr)
		{
			return;
		}
		std::ofstream out(result);
		out << GetMinPathCounts();
	}

	unsigned m_matrixRowsCount;
	unsigned m_matrixColumnsCount;
	matrix<std::pair<int, int>> m_landfill;
	matrix<sf::RectangleShape> rects;
	std::unique_ptr<CAppMenu> m_menu;
	std::queue<std::pair<std::pair<int, int>, int>> m_waveQueue;
};

int main()
{
	std::ifstream in("input.txt");

	unsigned M, N;
	in >> M >> N;

	CLandfillAggregator m_landfill(M, N);
	m_landfill.InitFromFile(in);
	m_landfill.Draw();
	return 0;
}