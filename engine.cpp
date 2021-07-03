#include "engine.h"
#include "generate_polygon.h"
#include <Box2D/Collision/Shapes/b2CircleShape.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Particle/b2ParticleGroup.h>
#include <cmath>
#include <iostream>

Engine::Engine( Level *level ) : level( level ), world( b2Vec2( 0, -9.8f ) ), rocks(), soil(), facilities(), particle_system( nullptr ), water_groups(), removed_soil( 0 ), water_transported( 0 ), contact_listener( this ), passed( false )
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

	for ( const auto &facility : level->facilities ) {
		if ( facility.type == DESTINATION ) {
			b2BodyDef bd;
			auto body = world.CreateBody( &bd );
			b2CircleShape shape;
			shape.m_radius = 1.0f;
			shape.m_p = b2Vec2( facility.x * PHYSICAL_RATIO, facility.y * PHYSICAL_RATIO );
			body->CreateFixture( &shape, 0.1f );
			facilities.push_back( body );
		}
	}

	b2ParticleSystemDef psd;
	psd.radius = 0.15f;
	psd.pressureStrength = 0.03f;
	psd.viscousStrength = 0.2f;
	psd.surfaceTensionNormalStrength = 0.3f;
	psd.destroyByAge = false;
	particle_system = world.CreateParticleSystem( &psd );

	for ( const auto &reservoir : level->reservoirs ) {
		b2ParticleGroupDef pgd;
		b2PolygonShape reservoir_shape;
		reservoir_shape.SetAsBox( reservoir.w * 0.5f * PHYSICAL_RATIO, reservoir.h * 0.5f * PHYSICAL_RATIO, b2Vec2( reservoir.x * PHYSICAL_RATIO, reservoir.y * PHYSICAL_RATIO ), 0 );
		pgd.shape = &reservoir_shape;
		pgd.flags |= b2_tensileParticle | b2_viscousParticle | b2_fixtureContactListenerParticle;
		water_groups.push_back( particle_system->CreateParticleGroup( pgd ) );
	}

	water_requested = particle_system->GetParticleCount() * 0.7;

	world.SetContactListener( &contact_listener );
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

	if ( passed ) {
		return;
	}

	// Handle excavate
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

	// Remove particle
	water_transported += removed_particle.size();
	while ( !removed_particle.empty() ) {
		particle_system->DestroyParticle( removed_particle.front(), false );
		removed_particle.pop();
	}
	if ( water_transported >= water_requested ) {
		water_transported = water_requested;
		passed = true;
	}

	// Simulation step
	world.Step( time_step, velocity_iteration, position_iteration, particle_iteration );
}
void Engine::ContactListener::BeginContact( b2ParticleSystem *particleSystem, b2ParticleBodyContact *particleBodyContact )
{
	auto particle_index = particleBodyContact->index;
	auto body = particleBodyContact->body;
	for ( auto f : super->facilities ) {
		if ( f == body ) {
			super->removed_particle.push( particle_index );
		}
	}
}
