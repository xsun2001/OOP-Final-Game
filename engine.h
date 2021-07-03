#pragma once
#include "level.h"
#include <Box2D/Collision/Shapes/b2ChainShape.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2World.h>
#include <queue>

const static float32 PHYSICAL_RATIO = 0.05;
const static uint32_t REBUILD_THRESHOLD = 50;
const static uint16_t DIG_RADIUS = 32;

class Game;

struct Point;

class Engine
{
	Level *level;
	b2World world;
	std::vector<b2Body *> rocks, soil, facilities;
	std::vector<b2ChainShape *> rock_shapes, soil_shapes;
	b2ParticleSystem *particle_system;
	std::vector<b2ParticleGroup *> water_groups;
	uint32_t removed_soil;
	uint32_t water_transported, water_requested;
	std::queue<int> removed_particle;
	bool passed;

	void tryEmptySoil( int x, int y );

	class ContactListener : public b2ContactListener
	{
		Engine *super;

	public:
		explicit ContactListener( Engine *super ) : super( super ){};
		void BeginContact( b2ParticleSystem *particleSystem, b2ParticleBodyContact *particleBodyContact ) override;
	} contact_listener;

public:
	friend class Game;
	friend class ContactListener;
	explicit Engine( Level *level );
	Level *getLevel();
	void tryExcavate( uint16_t x, uint16_t y );
	void stepping();
};