#include "level.h"
#include <cmath>

void splitUint32( uint32_t a, uint16_t &b1, uint16_t &b2 )
{
	b1 = a | 0x0000FFFF;
	b2 = a | 0xFFFF0000;
}

void mergeUint32( uint16_t b1, uint16_t &b2, uint32_t &a )
{
	a = ( (uint32_t) b2 << 16 ) | b1;
}

Bitmap::Bitmap( const Bitmap &other )
{
	memcpy( storage, other.storage, sizeof( storage ) );
}

Level *loadLevel( const std::string &path )
{
	FILE *file = fopen( path.c_str(), "rb" );
	auto *level = new Level;

	for ( auto &i : level->soil_map.storage ) {
		for ( auto &j : i ) {
			j = fgetc( file );
		}
	}

	for ( auto &i : level->rock_map.storage ) {
		for ( auto &j : i ) {
			j = fgetc( file );
		}
	}

	int facility_count = fgetc( file );
	for ( int i = 0; i < facility_count; i++ ) {
		Facility f{};
		splitUint32( getc( file ), reinterpret_cast<uint16_t &>( f.type ), f.x );
		splitUint32( getc( file ), f.y, f.arg );
		level->facilities.push_back( f );
	}

	int reservoirs_count = fgetc( file );
	for ( int i = 0; i < reservoirs_count; i++ ) {
		Reservoir r{};
		splitUint32( getc( file ), r.x, r.y );
		splitUint32( getc( file ), r.w, r.h );
		level->reservoirs.push_back( r );
	}

	return level;
}

Level *demoLevel()
{
	auto *level = new Level;
	level->soil_map.clear();
	level->rock_map.clear();
	for ( int i = 0; i < LEVEL_WIDTH; ++i ) {
		int maxj = std::min( uint32_t( pow( i - 320.0, 2 ) / 300.0 + 500.0 ), LEVEL_HEIGHT );
		for ( int j = 0; j < maxj; ++j ) {
			level->soil_map.set( i, j );
		}
	}
	for ( int i = 0; i < 20; ++i ) {
		for ( int j = 0; j < LEVEL_HEIGHT; ++j ) {
			level->rock_map.set( i, j );
			level->rock_map.set( LEVEL_WIDTH - i - 1, j );
		}
	}
	level->reservoirs.push_back( { 320, 900, 400, 100 } );
	level->facilities.push_back( { DESTINATION, 320, 100, 90 } );
	return level;
}

bool Bitmap::test( uint32_t x, uint32_t y )
{
	uint32_t x1 = x & 0b11111, x2 = x >> 5;
	return ( storage[y][x2] & ( 1 << x1 ) ) != 0;
}
void Bitmap::set( uint32_t x, uint32_t y )
{
	uint32_t x1 = x & 0b11111, x2 = x >> 5;
	storage[y][x2] |= ( 1 << x1 );
}
void Bitmap::unset( uint32_t x, uint32_t y )
{
	uint32_t x1 = x & 0b11111, x2 = x >> 5;
	storage[y][x2] &= ~( 1 << x1 );
}
void Bitmap::clear()
{
	memset( storage, 0, sizeof( storage ) );
}
std::optional<std::tuple<int, int>> Bitmap::find()
{
	for ( int i = 0; i < LEVEL_HEIGHT; ++i ) {
		for ( int j = 0; j < ( LEVEL_WIDTH >> 5 ); ++j ) {
			auto n = storage[i][j];
			if ( n != 0 ) {
				for ( int k = 0; k < 32; ++k ) {
					if ( n & ( 1 << k ) ) {
						return { { ( j << 5 ) | k, i } };
					}
				}
			}
		}
	}
	return std::nullopt;
}
