//	--------------------------------------------------------
//	TILESET.H
//	--------------------------------------------------------
//	Defined as a texture with a function to clip it to the relevant tile. 
//	Also declares global constant values for tile size and set width.
//	--------------------------------------------------------

#ifndef SRC_TILESET
#define SRC_TILESET

//	--------------------------------------------------------
//	Include
//	--------------------------------------------------------

#include <cstdint>
#include <iostream>

#include <SFML/Graphics.hpp>

//	--------------------------------------------------------
//	Some definitions
//	--------------------------------------------------------

#define TILE_SIZE											16
#define TILESET_WIDTH										16

//	--------------------------------------------------------
//	Main class
//	--------------------------------------------------------

class Tileset
{
private:
	sf::Texture	set_;

public:
	// Harcoded tile positions
	static const std::uint16_t WALL_TOPLEFT					= 0;
	static const std::uint16_t WALL_TOP						= WALL_TOPLEFT + 1;
	static const std::uint16_t WALL_TOPRIGHT				= WALL_TOPLEFT + 2;
	static const std::uint16_t WALL_LEFT					= TILESET_WIDTH;
	static const std::uint16_t FLOOR_TEXTURE				= TILESET_WIDTH + 1;
	static const std::uint16_t WALL_RIGHT					= TILESET_WIDTH + 2;
	static const std::uint16_t WALL_BOTTOMLEFT				= TILESET_WIDTH * 2;
	static const std::uint16_t WALL_BOTTOM					= WALL_BOTTOMLEFT + 1;
	static const std::uint16_t WALL_BOTTOMRIGHT				= WALL_BOTTOMLEFT + 2;
	static const std::uint16_t WALL_BR_CORNER				= 3;
	static const std::uint16_t WALL_BL_CORNER				= WALL_BR_CORNER + 1;
	static const std::uint16_t WALL_TR_CORNER				= TILESET_WIDTH + 3;
	static const std::uint16_t WALL_TL_CORNER				= WALL_TR_CORNER + 1;
	static const std::uint16_t WALL_TR_BL					= TILESET_WIDTH * 2 + 3;
	static const std::uint16_t WALL_TL_BR					= WALL_TR_BL + 1;
	static const std::uint16_t WALL_TEXTURE					= 5;

	Tileset();
	Tileset(sf::Image& image);

	sf::Sprite getTile(int tileIndex, sf::Vector2f pos);
	void Build(sf::Image& image);
};

//	--------------------------------------------------------
//	Constructors
//	--------------------------------------------------------

Tileset::Tileset()
{
	// asdfghjkl;
}

Tileset::Tileset(sf::Image& image)
{
	// Run sanity checks on the image reference and build it if our source is valid
	sf::Vector2u size = image.getSize();

	if (size.x != TILE_SIZE * TILESET_WIDTH)
	{
		std::cerr << "Error! Tileset width invalid; should be TILE_WIDTH * TILESET_WIDTH." << std::endl;
	}
	else if (size.y % TILE_SIZE != 0)
	{
		std::cerr << "Error! Tileset height not integer multiple of tile height." << std::endl;
	}
	else
	{
		set_;
		set_.loadFromImage(image);
	}
}

//	--------------------------------------------------------
//	Member functions
//	--------------------------------------------------------

void Tileset::Build(sf::Image& image)
{
	// Run sanity checks on the image reference and build it if our source is valid
	sf::Vector2u size = image.getSize();

	if (size.x != TILE_SIZE * TILESET_WIDTH)
	{
		std::cerr << "Error! Tileset width invalid; should be TILE_WIDTH * TILESET_WIDTH." << std::endl;
	}
	else if (size.y % TILE_SIZE != 0)
	{
		std::cerr << "Error! Tileset height not integer multiple of tile height." << std::endl;
	}
	else
	{
		set_;
		set_.loadFromImage(image);
	}
}

sf::Sprite Tileset::getTile(int tileIndex, sf::Vector2f pos)
{
	// Build a constraint rectangle and return the clipped image as a sprite
	// Sprites hold Texture* members, so it only points back to this
	int x = tileIndex % TILESET_WIDTH;
	int y = tileIndex / TILESET_WIDTH;

	int left = x * TILE_SIZE;
	int top = y * TILE_SIZE;

	sf::IntRect tileBounds(left, top, TILE_SIZE, TILE_SIZE);

	sf::Sprite ret(set_, tileBounds);
	ret.setPosition(pos);
	return ret;
}

//	--------------------------------------------------------

#endif