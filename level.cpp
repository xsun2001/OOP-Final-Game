#include "level.h"

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

bool Bitmap::test( uint32_t x, uint32_t y )
{
	uint32_t x1 = x & 0b1111, x2 = x >> 4;
	return ( storage[y][x2] & ( 1 << x1 ) ) != 0;
}
void Bitmap::set( uint32_t x, uint32_t y )
{
	uint32_t x1 = x & 0b1111, x2 = x >> 4;
	storage[y][x2] |= ( 1 << x1 );
}
void Bitmap::unset( uint32_t x, uint32_t y )
{
	uint32_t x1 = x & 0b1111, x2 = x >> 4;
	storage[y][x2] &= ~( 1 << x1 );
}
void Bitmap::clear()
{
	memset( storage, 0, sizeof( storage ) );
}
