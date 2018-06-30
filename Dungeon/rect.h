//	--------------------------------------------------------
//	RECT.H
//	--------------------------------------------------------
//	Extension of the SFML Rect class
//	--------------------------------------------------------

#ifndef RECT_H
#define RECT_H

//	--------------------------------------------------------

#include "edge.h"
#include <random>
#include "math.h"
#include <unordered_set>
#include <iostream>
#include <SFML/Graphics/Rect.hpp>

//	--------------------------------------------------------

class Rect : public sf::Rect<int>
{
public:
	Rect() : sf::Rect<int>() {};
	Rect(int a, int b, int c, int d) : sf::Rect<int>(a, b, c, d) { };

	void moveLeft(int x) { left += x; };
	void moveTop(int y) { top += y; };

	// Comparator for when we put this in a std::map
	friend bool operator<(const Rect& l, const Rect& r) {
		if (l.top < r.top)
		{
			return true;
		}
		else if (l.top > r.top)
		{
			return false;
		}
		else
		{
			if (l.left < r.left)
			{
				return true;
			}
			else if (l.left > r.left)
			{
				return false;
			}
			else
			{
				if (l.top + l.height < r.top + r.height)
				{
					return true;
				}
				else if (l.top + l.height > r.top + r.height)
				{
					return false;
				}
				else
				{
					return (l.left + l.width < r.left + r.width);
				}
			}
		}
	};

	static inline Vert centroid(Rect rect)				{ return Vert(rect.left + rect.width / 2, rect.top + rect.height / 2); };
};

//	--------------------------------------------------------

// Hash function extended for this type

namespace std
{
	template<> struct hash<Rect>
	{
		std::size_t operator()(Rect const& r) const
		{
			std::size_t const h1(std::hash<int>{}(r.left));
			std::size_t const h2(std::hash<int>{}(r.top));
			std::size_t const h3(std::hash<int>{}(r.width));
			std::size_t const h4(std::hash<int>{}(r.height));
			return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
		}
	};
}

//	--------------------------------------------------------

#endif