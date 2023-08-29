#pragma once

#include "Entity.h"
#include "EntityManager.h"

#include <SFML/Graphics.hpp>

/* Player Specification
	Shape Radius		SR			int
	Collision Radius	CR			int
	Speed				S			float
	Fill Color			FR,FG,FB	int,int,int
	Outline Color		OR,OG,OB	int,int,int
	Outline Thickness	OV			int
	Shape Vertices		V			int
*/
struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
/* Enemy Specification
	Shape Radius		SR			int
	Collision Radius	CR			int
	Speed				SMIN,SMAX	float, float
	Outline Color		OR,OG,OB	int,int,int
	Outline Thickness	OV			int
	Min/Max Vertices	VMIN,VMAX	int, int
	Small Lifespan		L			int
	Spawn Interval		SP			int
*/
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Game
{
	sf::RenderWindow m_window;
	int m_wWidth = 200;
	int m_wHeight = 200;
	EntityManager m_entities;
	sf::Font m_font;
	sf::Text m_text; // for the score
	PlayerConfig m_playerConfig;
	EnemyConfig m_enemyConfig;
	BulletConfig m_bulletConfig;
	int m_score = 0;
	int m_currentFrame = 0;
	int m_lastEnemySpawnTime = 0;
	bool m_paused = false;
	bool m_running = true;

	std::shared_ptr<Entity> m_player;

	void init(const std::string& config);
	void setPaused(bool paused);

	void sMovement();
	void sUserInput();
	void sLifespan();
	void sRender();
	void sEnemySpawner();
	void sCollision();

	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity> entity);
	void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos);
	void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

public:

	Game(const std::string& config);
	void run();

};

