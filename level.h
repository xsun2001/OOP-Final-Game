#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

const static uint32_t LEVEL_WIDTH = 640;
const static uint32_t LEVEL_HEIGHT = 960;

struct Level;

struct Bitmap {
	unsigned int storage[LEVEL_HEIGHT][LEVEL_WIDTH >> 5];
	Bitmap() = default;
	Bitmap( const Bitmap &other );
	bool test( uint32_t x, uint32_t y );
	void set( uint32_t x, uint32_t y );
	void unset( uint32_t x, uint32_t y );
	void clear();
	std::optional<std::tuple<int, int>> find();
};

enum FacilityType : uint16_t
{
	DESTINATION,
	FAUCET,
};

struct Facility {
	FacilityType type;
	uint16_t x, y, arg;
};

struct Reservoir {
	uint16_t x, y, w, h;
};

struct Level {
	Bitmap soil_map, rock_map;
	std::vector<Facility> facilities;
	std::vector<Reservoir> reservoirs;
};

Level *loadLevel( const std::string &path );

Level *demoLevel();
