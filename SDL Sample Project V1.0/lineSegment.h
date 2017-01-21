#pragma once
#include "vec2.h"
#include "line.h"
#include <algorithm>

class lineSegment {
public:
	vec2<float> begin;
	vec2<float> end;

	lineSegment::lineSegment();

	lineSegment::lineSegment(vec2<float> b, vec2<float> e);

	float distanceToPoint(vec2<float> p);

};