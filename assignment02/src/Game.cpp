#include "Game.h"

#include <iostream>
#include <fstream>
#include <time.h>
#include <stdlib.h>

Game::Game(const std::string& config)
{
	init(config);
}

void Game::init(const std::string& path)
{
	// initialize random number generator
	srand(time(NULL));

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
	fin >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;

	spawnPlayer();
}

void Game::run()
{
	while (m_running)
	{
		
		if (!m_paused)
		{
			m_entities.update();
			sEnemySpawner();
			sLifespan();
			sMovement();
			sCollision();
		}

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

	// colision shape
	entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

	// input
	entity->cInput = std::make_shared<CInput>();

	m_player = entity;
}

void Game::spawnEnemy()
{
	// TODO
	auto entity = m_entities.addEntity("enemy");

	// initialize the transform component
	float collisionRadius = m_enemyConfig.CR;
	float spawnX = randomRange(collisionRadius, m_wWidth - collisionRadius - 1);
	float spawnY = randomRange(collisionRadius, m_wHeight - collisionRadius - 1);
	Vec2 spawnPos(spawnX, spawnY);

	float angle = randomRange(0, 3.1415 * 2.0);
	Vec2 velocity(cos(angle), sin(angle));
	velocity *= randomRange(m_enemyConfig.SMIN, m_enemyConfig.SMAX);

	entity->cTransform = std::make_shared<CTransform>(spawnPos, velocity, 0.0f);

	// initialize the shape component
	float circleRadius = m_enemyConfig.SR;
	int vertices = randomRange(m_enemyConfig.VMIN, m_enemyConfig.VMAX);
	sf::Color fillColor(randomRange(0, 255), randomRange(0, 255), randomRange(0, 255));
	sf::Color outlineColor(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB);

	entity->cShape = std::make_shared<CShape>(circleRadius, vertices, fillColor, outlineColor, m_enemyConfig.OT);

	// initializer the collision component
	entity->cCollision = std::make_shared<CCollision>(collisionRadius);

	entity->cScore = std::make_shared<CScore>(vertices * 100);

	m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity)
{
	int vertices = entity->cShape->circle.getPointCount();
	for (int i = 0; i < vertices; i++)
	{
		auto smallEnemy = m_entities.addEntity("enemy");
		float angle = 360 / vertices * i;
		float angleRadians = angle * 3.1415 / 180;
		Vec2 spawnPos = entity->cTransform->pos;
		Vec2 velocity(cos(angleRadians), sin(angleRadians));


		sf::CircleShape enemyShape = entity->cShape->circle;
		int vertices = enemyShape.getPointCount();
		float circleRadius = enemyShape.getRadius() / 2;
		sf::Color fillColor = enemyShape.getFillColor();
		sf::Color outlineColor = enemyShape.getOutlineColor();
		int outlineThickness = enemyShape.getOutlineThickness();

		float collisionRadius = entity->cCollision->radius / 2;

		smallEnemy->cTransform = std::make_shared<CTransform>(spawnPos, velocity, 0.0f);
		smallEnemy->cShape = std::make_shared<CShape>
				(circleRadius, vertices, fillColor, outlineColor, outlineThickness);
		smallEnemy->cCollision = std::make_shared<CCollision>(collisionRadius);
		smallEnemy->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
		smallEnemy->cScore = std::make_shared<CScore>(vertices * 200);
	}
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
	auto bullet = m_entities.addEntity("bullet");

	Vec2 spawnPos = entity->cTransform->pos;
	Vec2 direction = target - spawnPos;
	direction.normalize();
	direction *= m_bulletConfig.S;

	bullet->cTransform = std::make_shared<CTransform>(spawnPos, direction, 0.0f);

	float circleRadius = m_bulletConfig.SR;
	int vertices = m_bulletConfig.V;
	sf::Color fillColor(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB);
	sf::Color outlineColor(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB);

	bullet->cShape = std::make_shared<CShape>(circleRadius, vertices, fillColor, outlineColor, m_bulletConfig.OT);

	bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);

	bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);

}

void Game::spawnSpecialWeapon(Vec2 target)
{
	auto clone = m_entities.addEntity("player");

	// give the entity a transform
	clone->cTransform = std::make_shared<CTransform>(target, Vec2(0, 0), 0);

	// entity shape
	sf::Color playerFillColor = sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB);
	sf::Color playerOutlineColor = sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB);
	clone->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V,
		playerFillColor, playerOutlineColor, m_playerConfig.OT);
	
	// colision shape
	clone->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

	// input
	clone->cInput = std::make_shared<CInput>();

	// lifespan
	clone->cLifespan = std::make_shared<CLifespan>(60 * 5);



}

void Game::sMovement()
{
	float move_x = m_player->cInput->right - m_player->cInput->left;
	float move_y = m_player->cInput->down - m_player->cInput->up;

	Vec2 velocity(move_x, move_y);
	velocity.normalize();
	velocity *= m_playerConfig.S;

	for (auto entity : m_entities.getEntities())
	{
		if (entity->cTransform)
		{
			if (entity->cInput)
			{
				entity->cTransform->velocity = velocity;
			}
			entity->cTransform->pos += entity->cTransform->velocity;
			entity->cShape->circle.setPosition(entity->cTransform->pos.x, entity->cTransform->pos.y);
		}
	}
}

void Game::sLifespan()
{
	for (auto entity : m_entities.getEntities())
	{
		if (entity->cLifespan)
		{
			entity->cLifespan->remaining -= 1;
			if (entity->cLifespan->remaining <= 0)
			{
				entity->destroy();
				continue;
			}
		}
	}
}

void Game::sCollision()
{
	// player
	if (m_player->cTransform->pos.x < m_player->cCollision->radius)
		m_player->cTransform->pos.x = m_player->cCollision->radius;
	if (m_player->cTransform->pos.y < m_player->cCollision->radius)
		m_player->cTransform->pos.y = m_player->cCollision->radius;
	if (m_player->cTransform->pos.x > m_wWidth - m_player->cCollision->radius - 1)
		m_player->cTransform->pos.x = m_wWidth - m_player->cCollision->radius - 1;
	if (m_player->cTransform->pos.y > m_wHeight - m_player->cCollision->radius - 1)
		m_player->cTransform->pos.y = m_wHeight - m_player->cCollision->radius - 1;

	// enemy bouncing
	for (auto enemy : m_entities.getEntities("enemy"))
	{
		if (enemy->cTransform->pos.x < enemy->cCollision->radius ||
			enemy->cTransform->pos.x > m_wWidth - enemy->cCollision->radius)
		{
			enemy->cTransform->velocity.x *= -1.0f;
		}
		if (enemy->cTransform->pos.y < enemy->cCollision->radius ||
			enemy->cTransform->pos.y > m_wHeight - enemy->cCollision->radius)
		{
			enemy->cTransform->velocity.y *= -1.0f;
		}
	}

	for (auto& b : m_entities.getEntities("bullet"))
	{
		for (auto& e : m_entities.getEntities("enemy"))
		{
			float dist = b->cTransform->pos.dist(e->cTransform->pos);
			if (dist < b->cCollision->radius + e->cCollision->radius)
			{
				m_score += e->cScore->score;
				b->destroy();
				if (!e->cLifespan)
				{
					spawnSmallEnemies(e);
				}
				e->destroy();
			}
		}
	}

	for (auto& player : m_entities.getEntities("player"))
	{
		for (auto& enemy : m_entities.getEntities("enemy"))
		{
			float dist = player->cTransform->pos.dist(enemy->cTransform->pos);
			if (dist < player->cCollision->radius + enemy->cCollision->radius)
			{
				if (!player->cLifespan)
				{
					spawnPlayer();
				}
				player->destroy();
				enemy->destroy();
			}
		}
	}
}

void Game::sEnemySpawner()
{
	if (m_currentFrame - m_lastEnemySpawnTime >= m_enemyConfig.SI)
	{
		spawnEnemy();
	}
}

void Game::sRender()
{
	m_window.clear();

	//std::cout << m_entities.getEntities().size() << std::endl;

	for (auto& e : m_entities.getEntities())
	{
		// set the position based on entity's transform->pos
		e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

		// set the rotation of the shape based on the transform angle
		e->cTransform->angle += 1.0f;
		e->cShape->circle.setRotation(e->cTransform->angle);

		//draw the entitiy's sf::CircleShape
		m_window.draw(e->cShape->circle);

		//std::cout << e->tag() << ": " << e->cTransform->velocity.x << ", " << e->cTransform->velocity.y << std::endl;

		if (e->cLifespan)
		{
			sf::Color currentColor = e->cShape->circle.getFillColor();
			float alpha = 255 * e->cLifespan->remaining / e->cLifespan->total;
			e->cShape->circle.setFillColor(sf::Color(currentColor.r, currentColor.g, currentColor.b, alpha));

			sf::Color currentOutline = e->cShape->circle.getOutlineColor();
			float outlineAlpha = 127 + 128 * e->cLifespan->remaining / e->cLifespan->total;
			e->cShape->circle.setOutlineColor
					(sf::Color(currentOutline.r, currentOutline.g, currentOutline.b, outlineAlpha));
		}
	}

	m_text.setString("Score: " + std::to_string(m_score));
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
				break;
			case sf::Keyboard::P:
				m_paused = m_paused ? false : true;
				break;
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

		if (event.type == sf::Event::MouseButtonPressed && !m_paused)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				for (auto& entity : m_entities.getEntities("player"))
				{
					spawnBullet(entity, Vec2(event.mouseButton.x, event.mouseButton.y));
				}
			}

			if (event.mouseButton.button == sf::Mouse::Right
					&& m_currentFrame - m_lastSpecialUsed > m_specialCountdown)
			{
				spawnSpecialWeapon(Vec2(event.mouseButton.x, event.mouseButton.y));
				m_lastSpecialUsed = m_currentFrame;
			}
		}
	}
}

int Game::randomRange(int min, int max)
{
	return min + rand() % (1 + max - min);
}