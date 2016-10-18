//	--------------------------------------------------------
//	RNG.H
//	--------------------------------------------------------
//	Class for holding the RNG
//	--------------------------------------------------------

#ifndef RNG_H
#define RNG_H

//	--------------------------------------------------------
//	Include
//	--------------------------------------------------------

#include <random>

#include "linal.h"
#include "rect.h"

//	--------------------------------------------------------
//	Main class
//	--------------------------------------------------------

class DungeonRNG
{
private:
	std::default_random_engine				generator_;		// RNG
	std::uniform_int_distribution<int>		die_;			// A die to roll for getting room sizes
	std::uniform_int_distribution<int>		rgb_;			// A random RGB
	std::uniform_real_distribution<float>	angle_;			// A random angle
public:
	// Static everything
	// Turns out the best way to do this is just to roll dice for the room dimensions
	static const int ROOM_DIE_SIZE = 3;
	static const int ROOM_DICE = 3;
	static const int ROOM_RADIUS = 5;

	static bool IsLarge(const Rect& r);

	DungeonRNG();

	int RoomDim();											// Generate random room dimension
	Rect GetRoom();											// Create a random room
	sf::Color GetColor();									// Generate a random color
};

DungeonRNG::DungeonRNG()
{
	// Set up the RNG
	generator_ = std::default_random_engine();
	die_ = std::uniform_int_distribution<int>(1, ROOM_DIE_SIZE);
	angle_ = std::uniform_real_distribution<float>(0, 2 * M_PI);
	rgb_ = std::uniform_int_distribution<int>(0, 255);
}

//	--------------------------------------------------------
//	RNG functions
//	--------------------------------------------------------

int DungeonRNG::RoomDim()
{
	int ret = 0;

	for (int i = 0; i < ROOM_DICE; i++)
	{
		ret += die_(generator_);
	}

	return ret;
}

Rect DungeonRNG::GetRoom()
{
	float theta = angle_(generator_);

	int x = (int)(ROOM_RADIUS * cos(theta));
	int y = (int)(ROOM_RADIUS * sin(theta));

	return Rect(x, y, RoomDim(), RoomDim());
}

sf::Color DungeonRNG::GetColor()
{
	return sf::Color(rgb_(generator_), rgb_(generator_), rgb_(generator_), 255);
}

//	--------------------------------------------------------
//	Static utility functions
//	--------------------------------------------------------

// Determines if the room should get triangulated
bool DungeonRNG::IsLarge(const Rect& r)
{
	float threshold = ((float)ROOM_DIE_SIZE / 1.8f * ROOM_DICE);
	return (r.width > threshold) && (r.height > threshold);
}

//	--------------------------------------------------------

#endif