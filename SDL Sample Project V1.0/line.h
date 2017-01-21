#pragma once
#include "vec2.h"
#include "lineSegment.h"
#include <algorithm>

class lineSegment;
class line;

class line {
public:
	vec2<float> direction;
	vec2<float> point;

	line::line();

	line::line(vec2<float> d, vec2<float> p);

	line::line(lineSegment a);

	bool segmentCollide(lineSegment a);

	vec2<float> collisionPoint(lineSegment a);

};