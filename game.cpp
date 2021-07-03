#include "game.h"
#include <SDL2_gfxPrimitives.h>

Game::Game() : Window( LEVEL_WIDTH, LEVEL_HEIGHT ), engine( demoLevel() )
{
}
void renderPolygon( SDL_Renderer *renderer, b2ChainShape *polygon, Uint8 r, Uint8 g, Uint8 b, Uint8 a )
{
	int32 count = polygon->GetChildCount();
	std::vector<Sint16> vx( count ), vy( count );
	for ( int i = 0; i < count; ++i ) {
		auto vert = polygon->m_vertices[i];
		vx[i] = Sint16( vert.x / PHYSICAL_RATIO );
		vy[i] = Sint16( LEVEL_HEIGHT - vert.y / PHYSICAL_RATIO );
	}
	filledPolygonRGBA( renderer, vx.data(), vy.data(), count, r, g, b, a );
}
void Game::render()
{
	engine.stepping();

	for ( const auto &soil : engine.soil_shapes ) {
		renderPolygon( renderer, soil, 0xFF, 0xFF, 0, 0xFF );
	}
	for ( const auto &rock : engine.rock_shapes ) {
		renderPolygon( renderer, rock, 0xFF, 0, 0, 0xFF );
	}
	filledCircleColor( renderer, LEVEL_WIDTH / 2, LEVEL_HEIGHT / 2, 100, 0x000000FF );
	for ( int i = 0; i < engine.particle_system->GetParticleCount(); ++i ) {
		const auto &particle = engine.particle_system->GetPositionBuffer()[i];
		filledCircleRGBA( renderer, particle.x / PHYSICAL_RATIO, LEVEL_HEIGHT - particle.y / PHYSICAL_RATIO, 4, 0, 0, 0xFF, 0xFF );
	}
}
void Game::start()
{
	show();
}
