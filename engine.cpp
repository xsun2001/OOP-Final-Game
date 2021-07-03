#include "engine.h"
#include "generate_polygon.h"
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Particle/b2ParticleGroup.h>
#include <cmath>

Engine::Engine( Level *level ) : level( level ), world( b2Vec2( 0, -9.8f ) ), rocks(), soil(), facilities(), particle_system( nullptr ), water_groups(), removed_soil( 0 )
{
	rock_shapes = std::move( createB2ShapesFromBitmap( level->rock_map ) );
	for ( const auto &rock_shape : rock_shapes ) {
		b2BodyDef bd;
		auto body = world.CreateBody( &bd );
		body->CreateFixture( rock_shape, 0.1f );
		rocks.push_back( body );
	}

	soil_shapes = std::move( createB2ShapesFromBitmap( level->soil_map ) );
	for ( const auto &soil_shape : soil_shapes ) {
		b2BodyDef bd;
		auto body = world.CreateBody( &bd );
		body->CreateFixture( soil_shape, 0.1f );
		soil.push_back( body );
	}

	// TODO: Handle facilities

	b2ParticleSystemDef psd;
	psd.radius = 0.15f;
	psd.destroyByAge = false;
	particle_system = world.CreateParticleSystem( &psd );

	for ( const auto &reservoir : level->reservoirs ) {
		b2ParticleGroupDef pgd;
		b2PolygonShape reservoir_shape;
		reservoir_shape.SetAsBox( reservoir.w * 0.5f * PHYSICAL_RATIO, reservoir.h * 0.5f * PHYSICAL_RATIO, b2Vec2( reservoir.x * PHYSICAL_RATIO, reservoir.y * PHYSICAL_RATIO ), 0 );
		pgd.shape = &reservoir_shape;
		pgd.flags |= b2_tensileParticle | b2_viscousParticle;
		water_groups.push_back( particle_system->CreateParticleGroup( pgd ) );
	}
}
Level *Engine::getLevel()
{
	return level;
}

void Engine::tryEmptySoil( int x, int y )
{
	if ( 0 <= x && x <= LEVEL_WIDTH && 0 <= y && y <= LEVEL_HEIGHT && ( removed_soil >= REBUILD_THRESHOLD || level->soil_map.test( x, y ) ) ) {
		++removed_soil;
		level->soil_map.unset( x, y );
	}
}
void Engine::tryExcavate( uint16_t x, uint16_t y )
{
	uint32_t R = DIG_RADIUS;
	for ( int i = 0; i <= R; ++i ) {
		for ( int j = sqrt( R * R - i * i ); j >= 0; --j ) {
			tryEmptySoil( x + j, y + i );
			tryEmptySoil( x + i, y - j );
			tryEmptySoil( x - j, y - i );
			tryEmptySoil( x - i, y + j );
		}
	}
}
void Engine::stepping()
{
	static auto time_step = 0.01666667f;
	static auto velocity_iteration = 6, position_iteration = 2, particle_iteration = 1;

	if ( removed_soil >= REBUILD_THRESHOLD ) {
		for ( const auto &old_body : soil ) {
			world.DestroyBody( old_body );
		}
		soil.clear();
		soil_shapes = std::move( createB2ShapesFromBitmap( level->soil_map ) );
		for ( const auto &soil_shape : soil_shapes ) {
			b2BodyDef bd;
			auto body = world.CreateBody( &bd );
			body->CreateFixture( soil_shape, 0.1f );
			soil.push_back( body );
		}
		removed_soil = 0;
	}

	world.Step( time_step, velocity_iteration, position_iteration, particle_iteration );
}
