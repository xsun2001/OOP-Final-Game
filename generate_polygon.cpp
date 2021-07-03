#include "generate_polygon.h"
#include <iostream>
#include <list>
#include <queue>
#include <tuple>
#include <vector>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

namespace BG = boost::geometry;
typedef BG::model::d2::point_xy<double> Point;
typedef BG::model::linestring<Point> Linestring;

void bfsOnBitmap( Bitmap &bm, int i, int j, std::list<Point> &points, std::tuple<int, int> &most_left_point )
{
	const static int dx[] = { -1, 0, 1, 0 }, dy[] = { 0, -1, 0, 1 };
	std::queue<std::tuple<int, int>> q;
	q.push( { i, j } );
	points.emplace_back( i, j );
	bm.unset( i, j );
	while ( !q.empty() ) {
		auto [x, y] = q.front();
		q.pop();
		if ( x < std::get<0>( most_left_point ) ) {
			most_left_point = { x, y };
		}
		for ( int k = 0; k < 4; k++ ) {
			int nx = x + dx[k], ny = y + dy[k];
			if ( nx < 0 || nx >= LEVEL_WIDTH || ny < 0 || ny >= LEVEL_HEIGHT ) {
				continue;
			}
			if ( bm.test( nx, ny ) ) {
				q.push( { nx, ny } );
				points.emplace_back( nx, ny );
				bm.unset( nx, ny );
			}
		}
	}
}

void findEdge( Bitmap &bm, Bitmap &vis, int sx, int sy, Linestring &edge )
{
	const static int dx[] = { -1, -1, 0, 1, 1, 1, 0, -1 }, dy[] = { 0, -1, -1, -1, 0, 1, 1, 1 };
	std::stack<std::tuple<int, int>> s;
	s.emplace( sx, sy );
	vis.set( sx, sy );
	edge.emplace_back( sx, sy );
	while ( !s.empty() ) {
		auto [x, y] = s.top();
		s.pop();
		for ( int i = 0; i < 8; ++i ) {
			auto nx = x + dx[i], ny = y + dy[i];
			if ( 0 <= nx && nx < LEVEL_WIDTH && 0 <= ny && ny <= LEVEL_HEIGHT && bm.test( nx, ny ) && !vis.test( nx, ny ) ) {
				bool is_edge = false;
				for ( int j = 0; j < 8; j += 2 ) {
					auto nnx = nx + dx[j], nny = ny + dy[j];
					if ( nnx < 0 || nnx >= LEVEL_WIDTH || nny < 0 || nny >= LEVEL_HEIGHT || !bm.test( nnx, nny ) ) {
						is_edge = true;
						break;
					}
				}
				if ( is_edge ) {
					s.emplace( nx, ny );
					vis.set( nx, ny );
					edge.emplace_back( nx, ny );
				}
			}
		}
	}
}

std::vector<b2ChainShape *> createB2ShapesFromBitmap( Bitmap &bm )
{
	std::vector<b2ChainShape *> shapes;
	Bitmap bm2{ bm }, vis{};
	vis.clear();
	for ( ;; ) {
		auto start_pos = bm2.find();
		if ( !start_pos ) {
			break;
		}
		auto [i, j] = *start_pos;
		auto most_left = *start_pos;
		std::list<Point> pg;
		bfsOnBitmap( bm2, i, j, pg, most_left );
		if ( pg.size() < 100 ) {
			// Ignore too small region
			continue;
		}

		Linestring edge, simplified;
		findEdge( bm, vis, std::get<0>( most_left ), std::get<1>( most_left ), edge );
		BG::simplify( edge, simplified, 1 );

		std::vector<b2Vec2> points;
		for ( const auto &p : simplified ) {
			points.emplace_back( p.x() * 0.05, p.y() * 0.05 );
		}
		auto shape = new b2ChainShape;
		shape->CreateLoop( points.data(), points.size() );
		shapes.push_back( shape );
	}
	return shapes;
}
