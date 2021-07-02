#include "game.h"
#include <SDL2_gfxPrimitives.h>

Game::Game() : Window( LEVEL_WIDTH, LEVEL_HEIGHT ), engine( demoLevel() )
{
}
void renderPolygon( SDL_Renderer *renderer, const b2ChainShape &polygon, Uint32 color )
{
	int32 count = polygon.GetChildCount();
	std::vector<Sint16> vx( count ), vy( count );
	for ( int i = 0; i < count; ++i ) {
		auto vert = polygon.m_vertices[i];
		vx[i] = Sint16( vert.x / PHYSICAL_RATIO );
		vy[i] = Sint16( LEVEL_HEIGHT - vert.y / PHYSICAL_RATIO );
	}
	filledPolygonColor( renderer, vx.data(), vy.data(), count, color );
}
void Game::render()
{
	engine.stepping();

	for ( const auto &soil : engine.soil_shapes ) {
		renderPolygon( renderer, soil, 0xFF0000FF );
	}
	for ( const auto &rock : engine.rock_shapes ) {
		renderPolygon( renderer, rock, 0xFFFF00FF );
	}

	for ( int i = 0; i < engine.particle_system->GetParticleCount(); ++i ) {
		const auto &particle = engine.particle_system->GetPositionBuffer()[i];
		filledCircleColor( renderer, particle.x / PHYSICAL_RATIO, particle.y / PHYSICAL_RATIO, 5, 0x0000FFFF );
	}
}
