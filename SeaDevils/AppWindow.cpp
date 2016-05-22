#include "stdafx.h"
#include "AppWindow.h"
#include "tinyfiledialogs.h"
#include "LandfillAggregator.h"
#include <fstream>

namespace
{
	const unsigned WINDOW_WIDTH = 800;
	const unsigned WINDOW_HEIGHT = 600;
	const unsigned MENU_BAR_HEIGHT = 20;
	const unsigned WINDOW_STYLE = sf::Style::Titlebar | sf::Style::Close;
	const int FRAME_SWITCH_INTERVAL = 1000;
}

CAppWindow::CAppWindow()
	: sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Prima Roads - RGR Sample", WINDOW_STYLE)
{
	m_menu = std::make_unique<CAppMenu>("File");
	m_menu->SetFrame(sf::FloatRect(0, 0, float(WINDOW_WIDTH), float(MENU_BAR_HEIGHT)));
	m_openActionId = m_menu->AddAction("Open...", std::bind(&CAppWindow::AskOpenInput, this));
	m_saveActionId = m_menu->AddAction("Save...", std::bind(&CAppWindow::AskSaveOutput, this));

	setFramerateLimit(60);
	setVerticalSyncEnabled(true);
	SetState(State::WaitingInput);
	m_font.loadFromFile("Sansation.ttf");
}

CAppWindow::~CAppWindow()
{
	m_menu.reset();
}

void CAppWindow::EnterLoop()
{
	while (isOpen())
	{
		sf::Event event;
		while (pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				close();
				return;
			}
			m_menu->OnEvent(event);
		}

		clear(sf::Color::Black);
		if (m_state != State::WaitingInput)
		{
			OnRunningDemo();
		}
		draw(*m_menu);
		display();
	}
}

void CAppWindow::SetState(CAppWindow::State state)
{
	m_state = state;
	m_menu->SetActionEnabled(m_openActionId, state != State::RunningDemo);
	m_menu->SetActionEnabled(m_saveActionId, state == State::WaitingOutput);
}

void CAppWindow::OnRunningDemo()
{
	if (m_state == State::RunningDemo && m_clock.getElapsedTime().asMilliseconds() >= FRAME_SWITCH_INTERVAL)
	{
		if (!SwitchNextFrame())
		{
			SetState(State::WaitingOutput);
		}
	}

	for (unsigned i = 0; i < m_visualElements->rects.size1(); i++)
	{
		for (unsigned j = 0; j < m_visualElements->rects.size2(); j++)
		{
			m_visualElements->texts(i, j).setString(std::to_string(m_aggregator->GetWaveLevel(i, j)));
			draw(m_visualElements->rects(i, j));
			draw(m_visualElements->texts(i, j));
		}
	}
}

void CAppWindow::RunAlgorithmDemo()
{
	m_aggregator->RunDevilWave();
	SetState(State::RunningDemo);

	for (unsigned i = 0; i < m_visualElements->rects.size1(); i++)
	{
		for (unsigned j = 0; j < m_visualElements->rects.size2(); j++)
		{
			sf::RectangleShape rect(sf::Vector2f(getSize().x / m_visualElements->rects.size1(), getSize().y / m_visualElements->rects.size2()));
			rect.setOutlineThickness(1.f);
			rect.setOutlineColor(sf::Color::Black);
			auto initValue = m_aggregator->GetWaveLevel(i, j);
			rect.setFillColor(initValue == -1 ? sf::Color::Red : initValue == 1 ? sf::Color::Green : sf::Color::Blue);
			rect.setPosition(sf::Vector2f(i * getSize().x / m_visualElements->rects.size1() + 1, j * getSize().y / m_visualElements->rects.size2() + 1));
			m_visualElements->rects(i, j) = rect;

			sf::Text text(std::to_string(m_aggregator->GetWaveLevel(i,j)), m_font, getSize().x / m_visualElements->texts.size1() / 2);
			text.setPosition(m_visualElements->rects(i, j).getPosition());
			text.setColor(sf::Color::Black);
			m_visualElements->texts(i, j) = text;
		}
	}
}

bool CAppWindow::SwitchNextFrame()
{
	m_clock.restart();
	if (!m_aggregator->SpreadDevilWave())
	{
		return false;
	}
	return true;
}

void CAppWindow::AskOpenInput()
{
	const char *filters[] = { "*.txt" };
	char const *result = tinyfd_openFileDialog("Select input file", "", 1, filters, "", false);

	if (result == nullptr)
	{
		return;
	}

	std::ifstream in(result);
	unsigned int m, n;
	in >> m >> n;
	m_aggregator = std::make_unique<CLandfillAggregator>(m,n);
	m_aggregator->InitFromFile(in);

	m_visualElements = std::make_unique<SVisualElements>(m_aggregator->GetDimensions().first, m_aggregator->GetDimensions().second);
	RunAlgorithmDemo();
}

void CAppWindow::AskSaveOutput()
{
	const char *filters[] = { "*.txt" };
	char const *result = tinyfd_saveFileDialog("Select output file", "", 1, filters, "");

	if (result == nullptr)
	{
		return;
	}
	std::ofstream out(result);
	out << m_aggregator->GetMinPathCounts();
}
