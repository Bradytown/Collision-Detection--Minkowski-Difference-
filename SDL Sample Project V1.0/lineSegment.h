#pragma once
#include "vec2.h"
#include "line.h"
#include <algorithm>
#include <SDL2_gfxPrimitives.h>

class lineSegment {
public:
	vec2<float> begin;
	vec2<float> end;

	lineSegment::lineSegment();

	lineSegment::lineSegment(vec2<float> b, vec2<float> e);

	float distanceToPoint(vec2<float> p);

	void draw(SDL_Renderer *rend);

	void moveTo(float x, float y);

	bool pointBelongsTo(vec2<float> p);

	bool collided(lineSegment v);

};