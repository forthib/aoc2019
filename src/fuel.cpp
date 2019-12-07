#include "fuel.h"

#include <algorithm>

namespace fuel
{
	size_t fromMass(size_t mass, bool includeFuelMass)
	{
		if (includeFuelMass)
		{
			size_t fuel = fromMass(mass, false);
			if (fuel > 0)
				fuel += fromMass(fuel, true);
			return fuel;
		}
		else
		{
			size_t fuel = mass / 3;
			fuel = std::max<size_t>(fuel, 2);
			fuel -= 2;
			return fuel;
		}
	}
}
