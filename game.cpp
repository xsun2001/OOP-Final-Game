#include "game.h"
#include <SDL2_gfxPrimitives.h>

Game::Game() : Window( LEVEL_WIDTH, LEVEL_HEIGHT ), engine( demoLevel() )
{
	auto mouse_handler = [&]( SDL_Event *event ) {
		int x, y;
		if ( SDL_GetMouseState( &x, &y ) & SDL_BUTTON( SDL_BUTTON_LEFT ) ) {
			engine.tryExcavate( x, LEVEL_HEIGHT - y );
		}
	};
	setEventHandler( SDL_MOUSEMOTION, mouse_handler );
	setEventHandler( SDL_MOUSEBUTTONDOWN, mouse_handler );
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
		renderPolygon( renderer, soil, 0xA8, 0x7E, 0x1D, 0xFF );
	}
	for ( const auto &rock : engine.rock_shapes ) {
		renderPolygon( renderer, rock, 0x40, 0x14, 0x4F, 0xFF );
	}
	filledCircleColor( renderer, LEVEL_WIDTH / 2, LEVEL_HEIGHT / 2, 100, 0x000000FF );
	for ( int i = 0; i < engine.particle_system->GetParticleCount(); ++i ) {
		const auto &particle = engine.particle_system->GetPositionBuffer()[i];
		filledCircleRGBA( renderer, particle.x / PHYSICAL_RATIO, LEVEL_HEIGHT - particle.y / PHYSICAL_RATIO, 4, 0, 0, 0xFF, 0xFF );
	}
	for ( const auto &facility : engine.level->facilities ) {
		if ( facility.type == DESTINATION ) {
			auto green = 0xFF * engine.water_transported / engine.water_requested;
			filledCircleRGBA( renderer, facility.x, LEVEL_HEIGHT - facility.y, 20, 0xFF - green, green, 0, 0xFF );
		}
	}

	if ( engine.passed ) {
	}
}
void Game::start()
{
	show();
}
