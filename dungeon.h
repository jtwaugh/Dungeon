//	--------------------------------------------------------
//	DUNGEON.H
//	--------------------------------------------------------
//	Generates an underlying mathematical structure that gets cached as a map
//	--------------------------------------------------------

#ifndef DUNGEON_H
#define DUNGEON_H

//	--------------------------------------------------------
//	Include
//	--------------------------------------------------------

#include "edge.h"
#include "topology.h"
#include "rng.h"

#include <cmath>
#include <unordered_set>
#include <iostream>

//	--------------------------------------------------------
//	We need corridors to know their orientation
//	--------------------------------------------------------

class Corridor : public Rect
{
private:
	bool horizontal_;
public:
	Corridor(int a, int b, int c, int d, bool _horizontal);
	bool horizontal() { return horizontal_; };
};

Corridor::Corridor(int a, int b, int c, int d, bool _horizontal) : Rect(a, b, c, d), horizontal_(_horizontal)
{

}

//	--------------------------------------------------------
//	Class for generating rooms and migrating them
//	--------------------------------------------------------

class Dungeon
{
private:
	// At its core, a dungeon is the following data:
	// Rectangle shapes that we guarantee won't overlap
	std::unordered_set<Rect>				rooms_;			// Set of rooms to remove duplicates
	std::vector<Corridor>					corridors_;		// Set of hallways

	// And an RNG
	DungeonRNG								rng_;

	// We track the center of mass
	// Not sure why this is important
	float									centerX_;
	float									centerY_;

	// We'll also cache the bounds of the dungeon for the hell of it
	int top_;
	int bottom_;
	int left_;
	int right_;

	// Resets the center coordinates
	void Center();

	// We also need to drift the rooms
	std::map<Rect, Vert> ZeroVelocity();
	void UpdateBounds(const Rect& r);
	void DriftIterate(std::map<Rect, Vert>& velocity);
	Vert DriftVector(Rect escapee, Rect collider);
	void Drift();
	void CreateCorridors();

public:
	static sf::RectangleShape FromRect(const Rect& r);
	static bool IsLarge(const Rect& r);

	// Constructor
	Dungeon();
	Dungeon(int roomsNum);

	// Accessors
	std::unordered_set<Rect> GetRooms()		{ return rooms_; };
	std::vector<Corridor> GetCorridors()	{ return corridors_; };

	int top()								{ return top_; };
	int bottom()							{ return bottom_; };
	int left()								{ return left_; };
	int right()								{ return right_; };

	bool CollisionsExist();

	// Generation functions
	void GenerateRooms(int n);
};

//	--------------------------------------------------------
//	Constructors
//	--------------------------------------------------------

// Default
Dungeon::Dungeon()
{
	rooms_ = std::unordered_set<Rect>();
	corridors_ = std::vector<Corridor>();
	rng_ = DungeonRNG();
}

// Generate n rooms
Dungeon::Dungeon(int roomsNum)
{
	rooms_ = std::unordered_set<Rect>();
	corridors_ = std::vector<Corridor>();
	rng_ = DungeonRNG();

	top_ = 0;
	bottom_ = 0;
	left_ = 0;
	right_ = 0;

	GenerateRooms(roomsNum);
	Drift();
	CreateCorridors();
}

void Dungeon::GenerateRooms(int n)
{	
	for (int i = 0; i < n; i++)
	{
		rooms_.insert(rng_.GetRoom());
	}

	Drift();
}

//	--------------------------------------------------------
//	Some utility functions
//	--------------------------------------------------------

void Dungeon::Center()
{
	centerX_ = WINDOW_WIDTH / 2;
	centerY_ = WINDOW_HEIGHT / 2;

	for (auto r = rooms_.begin(); r != rooms_.end(); r++)
	{
		centerX_ += r->left + (float)r->width / 2;
		centerY_ += r->top + (float)r->height / 2;
	}

	centerY_ /= rooms_.size();
	centerX_ /= rooms_.size();
}

// Builds a renderable shape from a room
sf::RectangleShape Dungeon::FromRect(const Rect& r)
{
	sf::RectangleShape renderRect(sf::Vector2f(r.width * TILE_SIZE, r.height * TILE_SIZE));
	renderRect.setPosition(FromTileCoords(r.left, r.top));
	return renderRect;
}

//	--------------------------------------------------------
//	Functions for making rooms drift
//	--------------------------------------------------------

// Checks each room against each other for collision
// I'm pretty sure we can't do better than O(N^2) on this guy
bool Dungeon::CollisionsExist()
{
	for (auto i = rooms_.begin(); i != rooms_.end(); i++)
	{
		for (auto j = rooms_.begin(); j != rooms_.end(); j++)
		{
			if (i != j && i->intersects(*j))
			{
				return true;
			}
		}
	}

	return false;
}

// Get the direction that a room will move away from its intersector
Vert Dungeon::DriftVector(Rect escapee, Rect collider)
{
	// Find the nearest edge

	// Distances
	// Careful; SFML flips coordinates
	int left = escapee.left - collider.left;
	int right = (collider.left + collider.width) - (escapee.left + escapee.width);
	int up = escapee.top - collider.top;
	int down = (collider.top + collider.height) - (escapee.top + escapee.height);

	// Find the centroids
	Vert ce = Rect::centroid(escapee);
	Vert cc = Rect::centroid(collider);
	
	// Get the collision area
	Rect in; 
	escapee.intersects(collider, in);

	// Round, if necessary
	float hor = (float)(in.width);
	float ver = (float)(in.height);

	// Decide the direction to move
	Direction dir = STAY;
	Direction oppDir = STAY;

	if (cc.x() > ce.x())
	{
		if (cc.y() > ce.y())
		{
			dir = (left < up) ? LEFT : UP;
			oppDir = (right > down) ? RIGHT : DOWN;
		}
		else
		{
			dir = (left < down) ? LEFT : DOWN;
			oppDir = (right > down) ? RIGHT : UP;
		}
	}
	else
	{
		if (cc.y() > ce.y())
		{
			dir = (right < up) ? RIGHT : UP;
			oppDir = (right > down) ? LEFT : DOWN;
		}
		else
		{
			dir = (right < down) ? RIGHT : DOWN;
			oppDir = (right > down) ? LEFT : UP;
		}
	}

	if ((dir == UP && oppDir == DOWN) || (dir == DOWN && oppDir == UP))
		ver /= 2;

	if ((dir == LEFT && oppDir == RIGHT) || (dir == RIGHT && oppDir == LEFT))
		hor /= 2;

	int h = ceil(hor);
	int v = ceil(ver);

	switch (dir)
	{
	case LEFT:
		return Vert(-h, 0);
	case RIGHT:
		return Vert(h, 0);
	case UP:
		return Vert(0, -v);
	case DOWN:
		return Vert(0, v);
	case STAY:
		return Vert(0, 0);
	}
}

void Dungeon::UpdateBounds(const Rect& r)
{
	top_ = (top_ > r.top) ? r.top : top_;
	bottom_ = (bottom_ < r.top + r.height) ? r.top + r.height : bottom_;
	left_ = (left_ > r.left) ? r.left : left_;
	right_ = (right_ < r.left + r.width) ? r.left + r.width : right_;
}

void Dungeon::DriftIterate(std::map<Rect, Vert>& velocity)
// Flock the rectangles apart until none of them touch
{
	// Because a bunch of rectangles will push a bunch of other rectangles, get the total pushing and sum it when we move them
	std::map<Rect, std::vector<Vert>> vectors;

	// For each rectangle
	for (auto r = rooms_.begin(); r != rooms_.end(); r++)
	{
		vectors[*r] = std::vector<Vert>();

		// For each other rectangle, if the two collide...
		for (auto s = rooms_.begin(); s != rooms_.end(); s++)
		{
			if (r->intersects(*s) && r != s)
			{
				// Get the distance it must travel toward the closest edge of the collider
				Vert d = DriftVector(*r, *s);
				vectors[*r].push_back(d);
				vectors[*s].push_back(Vert(d.x() * -1, d.y() * -1));
			}
		}
	}

	for (auto r = rooms_.begin(); r != rooms_.end(); r++)
	{
		// Average all the velocities
		if (vectors[*r].size() > 0)
		{
			float x = 0;
			float y = 0;

			auto myVector = vectors[*r];

			for (auto f = myVector.begin(); f != myVector.end(); f++)
			{
				x += f->x();
				y += f->y();
			}

			x = sgn(x);
			y = sgn(y);

			velocity[*r] = Vert(x, y);
		}
		else
		{
			velocity[*r] = Vert(0, 0);
		}
	}

	// To modify the set of rectangles, we have to move them into a new one
	// Ugh
	std::unordered_set<Rect> rooms;

	for (auto r = rooms_.begin(); r != rooms_.end(); r++)
	{
		// Move the room
		rooms.insert(Rect(r->left + velocity[*r].x(), r->top + velocity[*r].y(), r->width, r->height));
		UpdateBounds(*r);
		// Wipe the velocity
		velocity[*r] = Vert(0, 0);
	}

	// Memory leak?
	rooms_ = rooms;
}

std::map<Rect, Vert> Dungeon::ZeroVelocity()
{
	// Initialize the velocity table
	std::map<Rect, Vert> velocity;

	// Start it at zero
	for (auto r = rooms_.begin(); r != rooms_.end(); ++r)
	{
		velocity[*r] = Vert(0, 0);
	}

	int sizeBefore = rooms_.size();

	return velocity;
}

void Dungeon::Drift()
{
	std::map<Rect, Vert> velocity = ZeroVelocity();

	// While there are collisions
	while (CollisionsExist())
	{
		Center();

		// Try to resolve collisions
		DriftIterate(velocity);
	}
}

void Dungeon::CreateCorridors()
{
	int CORRIDOR_WIDTH = 3;

	std::vector<std::vector<float>> buffer;

	for (auto r = rooms_.begin(); r != rooms_.end(); r++)
	{
		if (DungeonRNG::IsLarge((*r)))
		{
			buffer.push_back({ Rect::centroid(*r).x(), Rect::centroid(*r).y() });
		}
	}

	std::sort(buffer.begin(), buffer.end());
	buffer.erase(std::unique(buffer.begin(), buffer.end()), buffer.end());

	// We'll get the MST but want to add some corridors back
	Delaunay del(buffer);
	auto tri = del.GetTriangulation();
	auto mst = del.GetMST();

	// For each corridor, go horizontal then vertical
	// Also remember the rooms we intersect
	std::unordered_set<Rect> hitRooms;
	
	for (auto c = mst.begin(); c != mst.end(); c++)
	{
		// Construct the horizontal corridor
		int x1 = floor((*c)->origin()->x());
		int x2 = floor((*c)->destination()->x());
		int y = floor((*c)->origin()->y());

		int left = x1;
		int right = x2;

		if (x1 > x2)
		{
			left = x2;
			right = x1;
		}

		corridors_.push_back(Corridor(left, y - 1, (right - left + 1), 3, true));

		// Construct the vertical corridor
		int y2 = floor((*c)->destination()->y());

		int top = y;
		int bottom = y2;

		if (y > y2)
		{
			top = y2;
			bottom = y;
		}

		corridors_.push_back(Corridor(x2 - 1, top, 3, (bottom - top + 1), false));

		for (auto r = rooms_.begin(); r != rooms_.end(); r++)
		{
			// If it's a large room, save it
			if (DungeonRNG::IsLarge(*r))
			{
				hitRooms.insert(*r);
			}

			// If the horizontal line segment intersects, then save the room
			if (r->top < y + 2 && r->top + r->height > y - 1 && r->left > left && r->left + r->width < right)
			{
				hitRooms.insert(*r);
			}

			// If the vertical line segment intersects, then save the room
			if (r->left < x2 + 2 && r->left + r->width > x2 - 1 && r->top > top && r->top + r->height < bottom)
			{
				hitRooms.insert(*r);
			}
		}
	}

	rooms_ = hitRooms;
}

//	--------------------------------------------------------
//	For drawing the wandering
//	--------------------------------------------------------

/*
void Dungeon::DriftAndRender(std::map<Rect, Vert>& velocity, sf::RenderWindow& window)
{
	Center();

	// Try to resolve collisions
	DriftIterate(velocity);

	// Create a renderable shape from each room and render it
	for (auto r = rooms_.begin(); r != rooms_.end(); r++)
	{
		sf::RectangleShape renderRect = FromRect((*r));
		if (DungeonRNG::IsLarge((*r)))
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

#endif