#include "Game.h"

#include <iostream>

int main()
{
	Vec2 vec1(6.0, 8.0);
	std::cout << vec1.length() << std::endl;
	vec1.normalize();
	std::cout << vec1.x << std::endl;
	Game g("config.txt");
	g.run();
}