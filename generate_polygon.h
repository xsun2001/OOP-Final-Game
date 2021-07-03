#pragma once

#include "level.h"
#include <Box2D/Collision/Shapes/b2ChainShape.h>
#include <vector>

std::vector<b2ChainShape *> createB2ShapesFromBitmap( Bitmap &bm );