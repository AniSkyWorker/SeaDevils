#pragma once
#include "stdafx.h"
#include "AppMenu.h"

class CLandfillAggregator;
using namespace boost::numeric::ublas;

struct SVisualElements
{
	SVisualElements(int i, int j) :texts(i, j), rects(i, j) {};
	matrix<sf::Text> texts;
	matrix<sf::RectangleShape> rects;
};
class CAppWindow : public sf::RenderWindow
{
public:
	CAppWindow();
	~CAppWindow();

	void EnterLoop();

private:
	enum class State
	{
		WaitingInput,
		RunningDemo,
		WaitingOutput,
	};

	void SetState(State state);
	void OnRunningDemo();
	void RunAlgorithmDemo();
	bool SwitchNextFrame();
	void AskOpenInput();
	void AskSaveOutput();

	State m_state = State::WaitingInput;
	sf::Clock m_clock;
	std::deque<std::string> m_pendingFramePaths;
	sf::Font m_font;
	std::unique_ptr<CLandfillAggregator> m_aggregator;
	std::unique_ptr<SVisualElements> m_visualElements;
	sf::Texture m_activeFrame;
	std::unique_ptr<CAppMenu> m_menu;
	
	size_t m_openActionId = 0;
	size_t m_saveActionId = 0;
};