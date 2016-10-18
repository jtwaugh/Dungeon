//	--------------------------------------------------------
//			MAP.H
//	--------------------------------------------------------
//	Contains actual tiles
//	--------------------------------------------------------

#ifndef MAP_H
#define MAP_H

//	--------------------------------------------------------
//	Include
//	--------------------------------------------------------

#include <cstdint>
#include <vector>
#include <tuple>

#include "tileset.h"
#include "dungeon.h"

//	--------------------------------------------------------
//	Before I drive myself crazy here
//	--------------------------------------------------------

typedef std::tuple<int, int> TilePos;

//	--------------------------------------------------------
//	Main class
//	--------------------------------------------------------

class Map
{
private:
	// Essential data: bounds, tileset, and tiles
	int							width_;
	int							height_;
	Tileset&					tileset_;
	std::uint16_t*				tiles_;

	// Utility functions
	void						AddTileAt(int x, int y, std::uint16_t tile);
	void						AddTileAt(TilePos t, std::uint16_t tile);
	void						TileRoom(int left, int top, const Rect& r);
	void						TileRoom(Dungeon& d, const Rect& r);

	// Bunch of geomtery; probably still has bugs
	std::uint16_t				CorridorTopWallRewrite(TilePos t);
	std::uint16_t				CorridorBottomWallRewrite(TilePos t);
	std::uint16_t				CorridorLeftWallRewrite(TilePos t);
	std::uint16_t				CorridorRightWallRewrite(TilePos t);
	std::uint16_t				CorridorTopLeftRewrite(TilePos t);
	std::uint16_t				CorridorTopRightRewrite(TilePos t);
	std::uint16_t				CorridorBottomLeftRewrite(TilePos t);
	std::uint16_t				CorridorBottomRightRewrite(TilePos t);

	void						FillWithFloor(Corridor& c, int left, int top);

	void						TileCorridor(int left, int top, Corridor& c);
	void						TileCorridor(Dungeon& d, Corridor& c);

public:
	// Accessors
	int							width()		{ return width_; };
	int							height()	{ return height_; };
	Tileset&					tileset()	{ return tileset_; };

	// Constructors
	Map();
	Map(int _width, int _height, Tileset& _tileset, std::uint16_t* _tiles);
	Map(Tileset& _tileset, Dungeon& _dungeon);

	// Utility functions
	sf::Sprite					GetTileAt(int x, int y);
	sf::Sprite					GetTileAt(TilePos t);
	std::uint16_t				GetTileTypeAt(int x, int y);
	std::uint16_t				GetTileTypeAt(TilePos t);

	// Game logic
	void						Update();
};

//	--------------------------------------------------------
//	Constructors
//	--------------------------------------------------------

//Default
Map::Map() : tileset_(Tileset())
{

}

// Direct construction
Map::Map(int _width, int _height, Tileset& _tileset, std::uint16_t* _tiles)
	: width_(_width),
	height_(_height),
	tileset_(_tileset),
	tiles_(_tiles)
{}

// Builds a map from a dungeon
Map::Map(Tileset& _tileset, Dungeon& _dungeon) : tileset_(_tileset)
{
	auto rooms = _dungeon.GetRooms();
	auto corridors = _dungeon.GetCorridors();

	width_ = _dungeon.right() - _dungeon.left();
	height_ = _dungeon.bottom() - _dungeon.top();

	// Bucket fill the grid with wall
	tiles_ = new std::uint16_t[width_ * height_];
	//memset(tiles_, Tileset::WALL_TEXTURE, width_ * height_);

	for (int y = 0; y < height_; y++)
	{
		for (int x = 0; x < width_; x++)
		{
			tiles_[x + y * width_] = Tileset::WALL_TEXTURE;
		}
	}

	// Now, for each room and corridor, offset it the correct amount and fill in the tiles
	for (auto r = rooms.begin(); r != rooms.end(); r++)
	{
		TileRoom(_dungeon, *r);
	}
	
	for (auto r = corridors.begin(); r != corridors.end(); r++)
	{
		TileCorridor(_dungeon, *r);
	}
}

//	--------------------------------------------------------
//	Accessors and mutators
//	--------------------------------------------------------

void Map::AddTileAt(int x, int y, std::uint16_t tile)
{
	// We use tile coordinates here
	tiles_[y * width_ + x] = tile;
}

void Map::AddTileAt(TilePos pos, std::uint16_t tile)
{
	AddTileAt(std::get<0>(pos), std::get<1>(pos), tile);
}

sf::Sprite Map::GetTileAt(int x, int y)
{
	// We'll factor for the camera before calling this, so these are world coordinates

	sf::Vector2f pos(TILE_SIZE * x, TILE_SIZE * y);
	std:uint16_t index = tiles_[x + y * width_];

	return tileset_.getTile(index, pos);
}

std::uint16_t Map::GetTileTypeAt(int x, int y)
{
	return tiles_[x + y * width_];
}

std::uint16_t Map::GetTileTypeAt(TilePos t)
{
	return GetTileTypeAt(std::get<0>(t), std::get<1>(t));
}

//	--------------------------------------------------------
//	Game logic
//	--------------------------------------------------------

void Map::Update()
{

}

//	--------------------------------------------------------
//	One layer up from geometry stuff
//	--------------------------------------------------------

void Map::TileRoom(int left, int top, const Rect& r)
{
	for (int y = 1; y < r.height - 1; y++)
	{
		for (int x = 1; x < r.width - 1; x++)
		{
			AddTileAt(r.left - left + x, r.top - top + y, Tileset::FLOOR_TEXTURE);
		}
	}

	// Corners
	AddTileAt(r.left - left, r.top - top, Tileset::WALL_TOPLEFT);
	AddTileAt(r.left - left + r.width - 1, r.top - top, Tileset::WALL_TOPRIGHT);
	AddTileAt(r.left - left, r.top - top + r.height - 1, Tileset::WALL_BOTTOMLEFT);
	AddTileAt(r.left - left + r.width - 1, r.top - top + r.height - 1, Tileset::WALL_BOTTOMRIGHT);

	// Top and bottom rows
	for (int i = 1; i < r.width - 1; i++)
	{
		AddTileAt(r.left - left + i, r.top - top, Tileset::WALL_TOP);
		AddTileAt(r.left - left + i, r.top - top + r.height - 1, Tileset::WALL_BOTTOM);
	}

	// Left and right columns
	for (int i = 1; i < r.height - 1; i++)
	{
		AddTileAt(r.left - left, r.top - top + i, Tileset::WALL_LEFT);
		AddTileAt(r.left - left + r.width - 1, r.top - top + i, Tileset::WALL_RIGHT);
	}
}

void Map::TileRoom(Dungeon& d, const Rect& r)
{
	TileRoom(d.left(), d.top(), r);
}

void Map::TileCorridor(int left, int top, Corridor& c)
{
	left = c.left - left;
	int right = left + c.width - 1;
	top = c.top - top;
	int bottom = top + c.height - 1;

	TilePos pos;

	if (c.horizontal())
	{
		// Add the left
		pos = std::make_tuple(left - 1, top);
		AddTileAt(pos, CorridorTopLeftRewrite(pos));
		pos = std::make_tuple(left - 1, top + 1);
		AddTileAt(pos, CorridorLeftWallRewrite(pos));
		pos = std::make_tuple(left - 1, top + 2);
		AddTileAt(pos, CorridorBottomLeftRewrite(pos));

		// Add the sides and center
		for (int x = 0; x < c.width - 1; x++)
		{
			pos = std::make_tuple(left + x, top);
			AddTileAt(pos, CorridorTopWallRewrite(pos));
		}

		FillWithFloor(c, left, top);

		for (int x = 0; x < c.width - 1; x++)
		{
			pos = std::make_tuple(left + x, bottom);
			AddTileAt(pos, CorridorBottomWallRewrite(pos));
		}

		// Add the right
		pos = std::make_tuple(left + c.width, top);
		AddTileAt(pos, CorridorTopRightRewrite(pos));
		pos = std::make_tuple(left + c.width, top + 1);
		AddTileAt(pos, CorridorRightWallRewrite(pos));
		pos = std::make_tuple(left+ c.width, top + 2);
		AddTileAt(pos, CorridorBottomRightRewrite(pos));
	}
	else
	{
		// Add the top
		pos = std::make_tuple(left, top - 1);
		AddTileAt(pos, CorridorTopLeftRewrite(pos));
		pos = std::make_tuple(left + 1, top - 1);
		AddTileAt(pos, CorridorTopWallRewrite(pos));
		pos = std::make_tuple(left + 2, top - 1);
		AddTileAt(pos, CorridorTopRightRewrite(pos));

		// Add the sides and center
		for (int y = 0; y < c.height - 1; y++)
		{
			pos = std::make_tuple(left, top + y);
			AddTileAt(pos, CorridorLeftWallRewrite(pos));
		}

		FillWithFloor(c, left, top);

		for (int y = 0; y < c.height - 1; y++)
		{
			pos = std::make_tuple(right, top + y);
			AddTileAt(pos, CorridorRightWallRewrite(pos));
		}

		// Add the bottom
		pos = std::make_tuple(left, top + c.height);
		AddTileAt(pos, CorridorBottomLeftRewrite(pos));
		pos = std::make_tuple(left + 1, top + c.height);
		AddTileAt(pos, CorridorBottomWallRewrite(pos));
		pos = std::make_tuple(left + 2, top + c.height);
		AddTileAt(pos, CorridorBottomRightRewrite(pos));
	}
}

void Map::TileCorridor(Dungeon& d, Corridor& r)
{
	TileCorridor(d.left(), d.top(), r);
}

//	--------------------------------------------------------
//	Geometry stuff
//	--------------------------------------------------------

std::uint16_t Map::CorridorTopWallRewrite(TilePos t)
{
	switch (GetTileTypeAt(t))
	{
	case Tileset::FLOOR_TEXTURE:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_LEFT:
		return Tileset::WALL_TL_CORNER;
	case Tileset::WALL_RIGHT:
		return Tileset::WALL_TR_CORNER;
	case Tileset::WALL_TL_CORNER:
		return Tileset::WALL_TL_CORNER;
	case Tileset::WALL_TR_CORNER:
		return Tileset::WALL_TR_CORNER;
	case Tileset::WALL_BL_CORNER:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_BR_CORNER:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_BOTTOM:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_BOTTOMRIGHT:
		return Tileset::WALL_TR_CORNER;
	case Tileset::WALL_BOTTOMLEFT:
		return Tileset::WALL_TL_CORNER;
	default:
		return Tileset::WALL_TOP;
	}
}

std::uint16_t Map::CorridorBottomWallRewrite(TilePos t)
{
	switch (GetTileTypeAt(t))
	{
	case Tileset::FLOOR_TEXTURE:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_LEFT:
		return Tileset::WALL_BL_CORNER;
	case Tileset::WALL_RIGHT:
		return Tileset::WALL_BR_CORNER;
	case Tileset::WALL_BL_CORNER:
		return Tileset::WALL_BL_CORNER;
	case Tileset::WALL_BR_CORNER:
		return Tileset::WALL_BR_CORNER;
	case Tileset::WALL_TL_CORNER:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_TR_CORNER:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_TOP:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_TOPLEFT:
		return Tileset::WALL_BL_CORNER;
	case Tileset::WALL_TOPRIGHT:
		return Tileset::WALL_BR_CORNER;
	default:
		return Tileset::WALL_BOTTOM;
	}
}

std::uint16_t Map::CorridorLeftWallRewrite(TilePos t)
{
	switch (GetTileTypeAt(t))
	{
	case Tileset::FLOOR_TEXTURE:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_TOP:
		return Tileset::WALL_TL_CORNER;
	case Tileset::WALL_BOTTOM:
		return Tileset::WALL_BL_CORNER;
	case Tileset::WALL_RIGHT:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_TL_CORNER:
		return Tileset::WALL_TL_CORNER;
	case Tileset::WALL_BL_CORNER:
		return Tileset::WALL_BL_CORNER;
	case Tileset::WALL_TR_CORNER:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_BR_CORNER:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_TOPRIGHT:
		return Tileset::WALL_TL_CORNER;
	case Tileset::WALL_BOTTOMRIGHT:
		return Tileset::WALL_BL_CORNER;
	default:
		return Tileset::WALL_LEFT;
	}
}

std::uint16_t Map::CorridorRightWallRewrite(TilePos t)
{
	switch (GetTileTypeAt(t))
	{
	case Tileset::FLOOR_TEXTURE:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_TOP:
		return Tileset::WALL_TR_CORNER;
	case Tileset::WALL_BOTTOM:
		return Tileset::WALL_BR_CORNER;
	case Tileset::WALL_LEFT:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_TR_CORNER:
		return Tileset::WALL_TR_CORNER;
	case Tileset::WALL_BR_CORNER:
		return Tileset::WALL_BR_CORNER;
	case Tileset::WALL_TL_CORNER:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_BL_CORNER:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_TOPLEFT:
		return Tileset::WALL_TR_CORNER;
	case Tileset::WALL_BOTTOMLEFT:
		return Tileset::WALL_BR_CORNER;
	default:
		std::cout << GetTileTypeAt(t) << std::endl;
		return Tileset::WALL_RIGHT;
	}
}

std::uint16_t Map::CorridorTopLeftRewrite(TilePos t)
{
	switch (GetTileTypeAt(t))
	{
	case Tileset::FLOOR_TEXTURE:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_TOP:
		return Tileset::WALL_TOP;
	case Tileset::WALL_LEFT:
		return Tileset::WALL_LEFT;
	case Tileset::WALL_RIGHT:
		return Tileset::WALL_TR_CORNER;
	case Tileset::WALL_TL_CORNER:
		return Tileset::WALL_TL_CORNER;
	case Tileset::WALL_TR_CORNER:
		return Tileset::WALL_TR_CORNER;
	case Tileset::WALL_BL_CORNER:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_BR_CORNER:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_BOTTOM:
		return Tileset::WALL_BL_CORNER;
	case Tileset::WALL_BOTTOMRIGHT:
		return Tileset::WALL_TR_BL;
	case Tileset::WALL_BOTTOMLEFT:
		return Tileset::WALL_TL_BR;
	default:
		return Tileset::WALL_TOPLEFT;
	}
}

std::uint16_t Map::CorridorTopRightRewrite(TilePos t)
{
	switch (GetTileTypeAt(t))
	{
	case Tileset::FLOOR_TEXTURE:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_TOP:
		return Tileset::WALL_TOP;
	case Tileset::WALL_RIGHT:
		return Tileset::WALL_RIGHT;
	case Tileset::WALL_LEFT:
		return Tileset::WALL_TL_CORNER;
	case Tileset::WALL_TL_CORNER:
		return Tileset::WALL_TL_CORNER;
	case Tileset::WALL_TR_CORNER:
		return Tileset::WALL_TR_CORNER;
	case Tileset::WALL_BL_CORNER:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_BR_CORNER:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_BOTTOM:
		return Tileset::WALL_BR_CORNER;
	case Tileset::WALL_BOTTOMRIGHT:
		return Tileset::WALL_TR_BL;
	case Tileset::WALL_BOTTOMLEFT:
		return Tileset::WALL_TL_BR;
	default:
		return Tileset::WALL_TOPRIGHT;
	}
}

std::uint16_t Map::CorridorBottomLeftRewrite(TilePos t)
{
	switch (GetTileTypeAt(t))
	{
	case Tileset::FLOOR_TEXTURE:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_BOTTOM:
		return Tileset::WALL_BOTTOM;
	case Tileset::WALL_LEFT:
		return Tileset::WALL_LEFT;
	case Tileset::WALL_TOP:
		return Tileset::WALL_TL_CORNER;
	case Tileset::WALL_RIGHT:
		return Tileset::WALL_BR_CORNER;
	case Tileset::WALL_TOPLEFT:
		return Tileset::WALL_LEFT;
	case Tileset::WALL_TL_CORNER:
		return Tileset::WALL_TL_CORNER;
	case Tileset::WALL_TR_CORNER:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_BL_CORNER:
		return Tileset::WALL_BL_CORNER;
	case Tileset::WALL_BR_CORNER:
		return Tileset::WALL_BR_CORNER;
	case Tileset::WALL_TOPRIGHT:
		return Tileset::WALL_TR_BL;
	default:
		return Tileset::WALL_BOTTOMLEFT;
	}
}

std::uint16_t Map::CorridorBottomRightRewrite(TilePos t)
{
	switch (GetTileTypeAt(t))
	{
	case Tileset::FLOOR_TEXTURE:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_BOTTOM:
		return Tileset::WALL_BOTTOM;
	case Tileset::WALL_RIGHT:
		return Tileset::WALL_RIGHT;
	case Tileset::WALL_TOP:
		return Tileset::WALL_TR_CORNER;
	case Tileset::WALL_LEFT:
		return Tileset::WALL_BL_CORNER;
	case Tileset::WALL_TOPRIGHT:
		return Tileset::WALL_RIGHT;
	case Tileset::WALL_TL_CORNER:
		return Tileset::FLOOR_TEXTURE;
	case Tileset::WALL_TR_CORNER:
		return Tileset::WALL_TR_CORNER;
	case Tileset::WALL_BL_CORNER:
		return Tileset::WALL_BL_CORNER;
	case Tileset::WALL_BR_CORNER:
		return Tileset::WALL_BR_CORNER;
	case Tileset::WALL_TOPLEFT:
		return Tileset::WALL_TL_BR;
	default:
		return Tileset::WALL_BOTTOMRIGHT;
	}
}

void Map::FillWithFloor(Corridor& c, int left, int top)
{
	for (int x = 1; x < c.width - 1; x++)
	{
		for (int y = 1; y < c.height - 1; y++)
		{
			TilePos pos = std::make_tuple(left + x, top + y);
			if (GetTileTypeAt(pos) != Tileset::FLOOR_TEXTURE)
			{
				AddTileAt(pos, Tileset::FLOOR_TEXTURE);
			}
		}
	}

	if (c.horizontal())
	{
		TilePos pos = std::make_tuple(left, top + 1);
		if (GetTileTypeAt(pos) != Tileset::FLOOR_TEXTURE)
		{
			AddTileAt(pos, Tileset::FLOOR_TEXTURE);
		}

		pos = std::make_tuple(left + c.width - 1, top + 1);
		if (GetTileTypeAt(pos) != Tileset::FLOOR_TEXTURE)
		{
			AddTileAt(pos, Tileset::FLOOR_TEXTURE);
		}
	}
	else
	{
		TilePos pos = std::make_tuple(left + 1, top);
		if (GetTileTypeAt(pos) != Tileset::FLOOR_TEXTURE)
		{
			AddTileAt(pos, Tileset::FLOOR_TEXTURE);
		}

		pos = std::make_tuple(left + 1, top + c.height - 1);
		if (GetTileTypeAt(pos) != Tileset::FLOOR_TEXTURE)
		{
			AddTileAt(pos, Tileset::FLOOR_TEXTURE);
		}
	}
}

//	--------------------------------------------------------

#endif