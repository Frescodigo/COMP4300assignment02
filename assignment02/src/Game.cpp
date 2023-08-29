#include "Game.h"

#include <iostream>
#include <fstream>

Game::Game(const std::string& config)
{
	init(config);
}

void Game::init(const std::string& path)
{
	// TODO read in from input file
	std::ifstream fin(path);
	std::string buffer;

	// get window parameters
	int framerate = 0;
	bool fullscreen = false;
	fin >> buffer >> m_wWidth >> m_wHeight >> framerate >> fullscreen;

	// set window parameters
	m_window.create(sf::VideoMode(m_wWidth, m_wHeight), "Assignment 2",
			fullscreen ? sf::Style::Fullscreen : sf::Style::Default);
	m_window.setFramerateLimit(framerate);

	// get font parameters
	std::string fontPath = "";
	int fontSize = 0;
	int fontR = 0, fontG = 0, fontB = 0;
	fin >> buffer >> fontPath >> fontSize >> fontR >> fontG >> fontB;

	// set font and text parameters
	if (!m_font.loadFromFile(fontPath))
	{
		// throw error
	}
	m_text.setFont(m_font);
	m_text.setCharacterSize(fontSize);
	m_text.setFillColor(sf::Color(fontR, fontG, fontB));
	m_text.setString("Score: 0");

	// read in player parameters
	// Player SR CR S FR FG FB OR OG OB OT V
	fin >> buffer >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S;
	fin >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB;
	fin >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB;
	fin >> m_playerConfig.OT >> m_playerConfig.V;

	// read in enemy parameters
	// Enemy SR CR SMIN SMAX OR OG OB OT VMIN VMAX L SI
	fin >> buffer >> m_enemyConfig.SR >> m_enemyConfig.CR;
	fin >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX;
	fin >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB;
	fin >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX;
	fin >> m_enemyConfig.L >> m_enemyConfig.SI;

	// read in bullet parameters
	// Bullet SR CR S FR FG FB OR OG OB OT V L
	fin >> buffer >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S;
	fin >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB;
	fin >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB;
	fin >> m_bulletConfig.V >> m_bulletConfig.L;

	spawnPlayer();
}

void Game::run()
{
	//while (m_running)
	//{
	//	// m_entities.update();

	//	sEnemySpawner();
	//	sMovement();
	//	sCollision();
	//	sUserInput();
	//	sRender();

	//	// may need to be moved
	//	m_currentFrame++;
	//}

	while (m_window.isOpen())
	{
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				m_window.close();
			}
		}

		m_window.clear();
		m_window.draw(m_text);
		m_window.display();
	}
}

void Game::setPaused(bool paused)
{
	m_paused = paused;
}

void Game::spawnPlayer()
{
	//// TODO add all properties to the player from the config file
	//auto entity = m_entities.addEntity("player");

	//// give the entity a transform
	////entity->cTransform = std::make_shared<CTransform>(Vec2(m_wWidth / 2.f, m_wHeight / 2.f), Vec2(0,0), 0);

	//// entity shape
	//sf::Color playerFillColor = sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB);
	//sf::Color playerOutlineColor = sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB);
	//entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V,
	//		playerFillColor, playerOutlineColor, m_playerConfig.OT);

	//// input
	//entity->cInput = std::make_shared<CInput>();

	//m_player = entity;
}