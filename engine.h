#pragma once
#include "level.h"
#include <Box2D/Collision/Shapes/b2ChainShape.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2World.h>

const static float32 PHYSICAL_WIDTH = 3.2;
const static float32 PHYSICAL_HEIGHT = 4.8;
const static float32 PHYSICAL_RATIO = 0.005;
const static uint32_t REBUILD_THRESHOLD = 200;
const static uint16_t DIG_RADIUS = 64;

class Game;

class Engine
{
	Level *level;
	b2World world;
	std::vector<b2Body *> rocks, soil, facilities;
	std::vector<b2ChainShape> rock_shapes, soil_shapes;
	b2ParticleSystem *particle_system;
	std::vector<b2ParticleGroup *> water_groups;
	uint32_t removed_soil;

	void tryEmptySoil( int x, int y );
	static std::vector<b2ChainShape> createB2ShapesFromBitmap( Bitmap bm );// Copy on invoke

public:
	friend class Game;

	explicit Engine( Level *level );
	Level *getLevel();
	void tryExcavate( uint16_t x, uint16_t y );
	void stepping();
};