#include "engine.h"
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Particle/b2ParticleGroup.h>
#include <boost/geometry.hpp>
#include <cmath>
#include <queue>

namespace bg = boost::geometry;

using Point = bg::model::point<float32, 2, bg::cs::cartesian>;
using PointGroup = bg::model::multi_point<Point>;
using Polygon = bg::model::polygon<Point>;

Engine::Engine( Level *level ) : level( level ), world( b2Vec2( 0, -9.8f ) ), rocks(), soil(), facilities(), particle_system( nullptr ), water_groups(), removed_soil( 0 )
{
	rock_shapes = std::move( createB2ShapesFromBitmap( level->rock_map ) );
	for ( const auto &rock_shape : rock_shapes ) {
		b2BodyDef bd;
		auto body = world.CreateBody( &bd );
		body->CreateFixture( &rock_shape, 0.1f );
		rocks.push_back( body );
	}

	soil_shapes = std::move( createB2ShapesFromBitmap( level->soil_map ) );
	for ( const auto &soil_shape : soil_shapes ) {
		b2BodyDef bd;
		auto body = world.CreateBody( &bd );
		body->CreateFixture( &soil_shape, 0.1f );
		rocks.push_back( body );
	}

	// TODO: Handle facilities

	b2ParticleSystemDef psd;
	psd.radius = 0.025f;
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

void bfsOnBitmap( Bitmap &bm, int i, int j, PointGroup &pg )
{
	const static int dx[] = { 0, 0, 1, -1 }, dy[] = { 1, -1, 0, 0 };
	std::queue<std::tuple<int, int>> q;
	q.push( { i, j } );
	bg::append( pg, Point( i * PHYSICAL_RATIO, j * PHYSICAL_RATIO ) );
	bm.unset( i, j );
	while ( !q.empty() ) {
		auto [x, y] = q.front();
		q.pop();
		for ( int k = 0; k < 4; k++ ) {
			int nx = x + dx[k], ny = y + dy[k];
			if ( bm.test( nx, ny ) ) {
				q.push( { nx, ny } );
				bg::append( pg, Point( nx * PHYSICAL_RATIO, ny * PHYSICAL_RATIO ) );
				bm.unset( nx, ny );
			}
		}
	}
}

std::vector<b2ChainShape> Engine::createB2ShapesFromBitmap( Bitmap bm )
{
	std::vector<b2ChainShape> shapes;
	std::vector<b2Vec2> points_cache;
	for ( ;; ) {
		bool found = false;
		int i = 0, j = 0;
		for ( ; i < LEVEL_WIDTH; ++i ) {
			for ( ; j < LEVEL_HEIGHT; ++j ) {
				if ( bm.test( i, j ) ) {
					found = true;
					break;
				}
			}
		}
		if ( !found ) {
			break;
		}
		PointGroup pg;
		bfsOnBitmap( bm, i, j, pg );
		if ( pg.size() < 100 ) {
			// Ignore too small region
			continue;
		}
		Polygon hull;
		bg::convex_hull( pg, hull );
		const auto &ring = hull.outer();
		int inserted_count = 0, cached_count = points_cache.size();
		for ( const auto &ring_point : ring ) {
			if ( inserted_count < cached_count ) {
				points_cache[inserted_count] = b2Vec2( ring_point.get<0>(), ring_point.get<1>() );
			} else {
				points_cache.emplace_back( ring_point.get<0>(), ring_point.get<1>() );
			}
			++inserted_count;
		}
		shapes.emplace_back().CreateChain( points_cache.data(), inserted_count );
	}
	return shapes;
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
	static auto velocity_iteration = 8, position_iteration = 3, particle_iteration = world.CalculateReasonableParticleIterations( time_step );

	if ( removed_soil >= REBUILD_THRESHOLD ) {
		for ( const auto &old_body : soil ) {
			world.DestroyBody( old_body );
		}
		soil.clear();
		soil_shapes = std::move( createB2ShapesFromBitmap( level->soil_map ) );
		for ( const auto &soil_shape : soil_shapes ) {
			b2BodyDef bd;
			auto body = world.CreateBody( &bd );
			body->CreateFixture( &soil_shape, 0.1f );
			soil.push_back( body );
		}
		removed_soil = 0;
	}

	world.Step( time_step, velocity_iteration, position_iteration, particle_iteration );
}
