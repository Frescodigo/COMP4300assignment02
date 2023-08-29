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
	while (m_running)
	{
		m_entities.update();

		//sEnemySpawner();
		sMovement();
		//sCollision();
		sUserInput();
		sRender();

		// may need to be moved
		m_currentFrame++;
	}
}

void Game::setPaused(bool paused)
{
	m_paused = paused;
}

void Game::spawnPlayer()
{
	// TODO add all properties to the player from the config file
	auto entity = m_entities.addEntity("player");

	// give the entity a transform
	entity->cTransform = std::make_shared<CTransform>(Vec2(m_wWidth / 2.f, m_wHeight / 2.f), Vec2(0,0), 0);

	// entity shape
	sf::Color playerFillColor = sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB);
	sf::Color playerOutlineColor = sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB);
	entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V,
			playerFillColor, playerOutlineColor, m_playerConfig.OT);

	// input
	entity->cInput = std::make_shared<CInput>();

	m_player = entity;
}

void Game::spawnEnemy()
{
	// TODO
	auto entity = m_entities.addEntity("enemy");

	float mx = 30;
	float my = 30;

	entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(0, 0), 0);

	entity->cShape = std::make_shared<CShape>(32, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);

	m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{

}

void Game::spawnBullet(std::shared_ptr<Entity> e, const Vec2& target)
{

}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{

}

void Game::sMovement()
{
	float move_x = m_player->cInput->right - m_player->cInput->left;
	float move_y = m_player->cInput->down - m_player->cInput->up;

	Vec2 velocity(move_x, move_y);
	velocity.normalize();
	velocity *= m_playerConfig.S;
	m_player->cTransform->velocity = velocity;

	for (auto entity : m_entities.getEntities())
	{
		if (entity->cTransform)
		{
			entity->cTransform->pos += entity->cTransform->velocity;
			entity->cShape->circle.setPosition(entity->cTransform->pos.x, entity->cTransform->pos.y);
		}
	}
}

void Game::sLifespan()
{

}

void Game::sCollision()
{

}

void Game::sEnemySpawner()
{

}

void Game::sRender()
{
	m_window.clear();

	std::cout << m_entities.getEntities().size() << std::endl;

	for (auto& e : m_entities.getEntities())
	{
		// set the position based on entity's transform->pos
		e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

		// set the rotation of the shape based on the transform angle
		e->cTransform->angle += 1.0f;
		e->cShape->circle.setRotation(e->cTransform->angle);

		//draw the entitiy's sf::CircleShape
		m_window.draw(e->cShape->circle);
	}

	m_window.draw(m_text);

	m_window.display();
}

void Game::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			m_running = false;
		}

		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Escape:
				m_running = false;
				break;
			case sf::Keyboard::W:
				m_player->cInput->up = true;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = true;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = true;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = true;
			default:
				break;
			}
		}

		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				m_player->cInput->up = false;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = false;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = false;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = false;
			default:
				break;
			}
		}
	}
}