//	--------------------------------------------------------
//	GAMEWORLD.H
//	--------------------------------------------------------
//	Class to contain the dungeon information, the tileset (?), and the camera position
//	--------------------------------------------------------

#ifndef GAMEWORLD_H
#define GAMEWORLD_H

//	--------------------------------------------------------
//	Include
//	--------------------------------------------------------

#include "dungeon.h"
#include "map.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>

//	--------------------------------------------------------
//	Struct to represent the camera
//	--------------------------------------------------------

struct Camera
{
public:
	int x;
	int y;

	Camera();
};

Camera::Camera()
{
	x = 0;
	y = 0;
}

//	--------------------------------------------------------
//	Main class
//	--------------------------------------------------------

class GameWorld
{
private:
	Map map_;
	Camera camera_;

	static const int CAMERA_SPEED = 2;

	void UpdateCamera();
	void RenderDungeon(sf::RenderWindow& window);
	void RenderMap(sf::RenderWindow& window);

public:
	GameWorld();
	GameWorld(Tileset& _tileset, int n);

	void Update();
	void Render(sf::RenderWindow& window);
};

//	--------------------------------------------------------
// Constructor
//	--------------------------------------------------------

GameWorld::GameWorld()
{
	camera_ = Camera();
}

GameWorld::GameWorld(Tileset& _tileset, int n) : map_(Map(_tileset, Dungeon(n)))
{
	camera_ = Camera();
}

//	--------------------------------------------------------
//	Game logic functions
//	--------------------------------------------------------

void GameWorld::Update()
{
	UpdateCamera();
}

void GameWorld::UpdateCamera()
{
	// Let WASD control the camera for now
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		if (camera_.y > 0)
		{
			camera_.y -= CAMERA_SPEED;
		}
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		if (camera_.y + WINDOW_HEIGHT < map_.height() * TILE_SIZE)
		{
			camera_.y += CAMERA_SPEED;
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		if (camera_.x > 0)
		{
			camera_.x -= CAMERA_SPEED;
		}
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		if (camera_.x + WINDOW_WIDTH < map_.width() * TILE_SIZE)
		{
			camera_.x += CAMERA_SPEED;
		}
	}
}

//	--------------------------------------------------------
//	Rendering functions
//	--------------------------------------------------------

void GameWorld::Render(sf::RenderWindow& window)
{
	RenderMap(window);
}

void GameWorld::RenderMap(sf::RenderWindow& window)
{
	int left = floor(camera_.x / TILE_SIZE);
	int top = floor(camera_.y / TILE_SIZE);

	int right = left + WINDOW_WIDTH / TILE_SIZE + 1;
	int bottom = top + WINDOW_HEIGHT / TILE_SIZE + 1;

	for (int y = top; y < bottom; y++)
	{
		for (int x = left; x < right; x++)
		{
			sf::Sprite s = map_.GetTileAt(x, y);
			s.move(-camera_.x, -camera_.y);
			window.draw(s);
		}
	}
}

/*
void GameWorld::RenderDungeon(sf::RenderWindow& window)
{
	auto corridors = dungeon_.GetCorridors();
	auto rooms = dungeon_.GetRooms();

	// Create a renderable shape from each corridor and render it
	for (auto r = corridors.begin(); r != corridors.end(); r++)
	{
		sf::RectangleShape renderRect = Dungeon::FromRect((*r));
		renderRect.move(-camera_.x, -camera_.y);
		renderRect.setFillColor(sf::Color::Red);
		window.draw(renderRect);
	}

	// Create a renderable shape from each room and render it
	for (auto r = rooms.begin(); r != rooms.end(); r++)
	{
		sf::RectangleShape renderRect = Dungeon::FromRect((*r));
		renderRect.move(-camera_.x, -camera_.y);

		if (Dungeon::IsLarge((*r)))
		{
			renderRect.setFillColor(sf::Color::White);
		}
		else
		{
			renderRect.setFillColor(sf::Color::Blue);
		}

		window.draw(renderRect);
	}
}
*/

//	--------------------------------------------------------
//	Miscellaneous debug stuff
//	--------------------------------------------------------

void DrawEdge(Edge* e, sf::RenderWindow& window, const sf::Color& color)
{
	Edge* sym = e->Sym();
	Vert* destination = e->destination();
	Vert org = *(e->origin());
	Vert dest = *(e->destination());

	sf::VertexArray v(sf::Lines, 2);

	v[0].position = sf::Vector2f(org.x(), org.y());
	v[0].color = color;
	v[1].position = sf::Vector2f(dest.x(), dest.y());
	v[1].color = color;

	window.draw(v);
}

void DrawDungeonEdge(Edge* e, sf::RenderWindow& window, const sf::Color& color)
{
	Edge* sym = e->Sym();
	Vert* destination = e->destination();
	Vert org = *(e->origin());
	Vert dest = *(e->destination());

	sf::VertexArray v(sf::Lines, 2);

	v[0].position = sf::Vector2f(org.x() * TILE_SIZE + WINDOW_WIDTH / 2, org.y() * TILE_SIZE + WINDOW_HEIGHT / 2);
	v[0].color = color;
	v[1].position = sf::Vector2f(dest.x() * TILE_SIZE + WINDOW_WIDTH / 2, dest.y() * TILE_SIZE + WINDOW_HEIGHT / 2);
	v[1].color = color;

	window.draw(v);
}

void DrawVoronoi(Edge* e, sf::RenderWindow& window)
{
	sf::VertexArray v(sf::Lines, 2);

	Vert* org = e[1].origin();
	Vert* dest = e[3].origin();

	v[0].position = sf::Vector2f(org->x(), org->y());
	v[0].color = sf::Color::Green;
	v[1].position = sf::Vector2f(dest->x(), dest->y());
	v[1].color = sf::Color::Green;

	window.draw(v);
}

//	--------------------------------------------------------

#endif

