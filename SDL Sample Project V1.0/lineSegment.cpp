#include "lineSegment.h"
#include <iostream>
#include <cmath>

lineSegment::lineSegment() {
	begin.x = 0;
	begin.y = 0;
	end.x = 1;
	end.y = 1;
}

lineSegment::lineSegment(vec2<float> b, vec2<float> e) {
	begin = b;
	end = e;
}

float lineSegment::distanceToPoint(vec2<float> p) {

	//Distance to point is working as intended for flat lines
	//Bug arises here from slanted lines

	vec2<float> lineSegmentDirection(end - begin);

	//check if line perpendicular to line segment originating from point intersects segment
	//if so, that distance is shortest
	line perp = line(vec2<float>(-lineSegmentDirection.y, lineSegmentDirection.x), p);

	//Bug originates from this if statement
	//Bug only affects lines that aren't aligned with one of the axis
	if (perp.segmentCollide(*this)) {
		vec2<float> collisionPoint = perp.collisionPoint(line(*this));
		return sqrt(pow(p.x - collisionPoint.x, 2) + pow(p.y - collisionPoint.y, 2));
	}
	//else, shortest distance is shortest between end points
	else {
		return std::min(vec2<float>(p - end).length(), vec2<float>(p - begin).length());
	}

}

void lineSegment::draw(SDL_Renderer *rend) {
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
	SDL_RenderDrawLine(rend, begin.x, begin.y, end.x, end.y);
}

void lineSegment::moveTo(float x, float y) {
	vec2<float> delta = end - begin;
	begin.x = x;
	begin.y = y;
	end = begin + delta;
}

bool lineSegment::pointBelongsTo(vec2<float> v) {
	line a = line(*this);
	vec2<float> perpDirection(v.y - a.point.y, a.point.x - v.x);
	
	float tol = std::pow(10,-3);

	if ((std::abs(vec2<float>::dot(a.direction, perpDirection)) < tol //100% Rigourously Proven
		&& v.x >= std::min(begin.x, end.x) && v.x <= std::max(begin.x, end.x)
		&& v.y >= std::min(begin.y, end.y) && v.y <= std::max(begin.y, end.y)) || 
		end == v ||
		begin == v) { 
		return true;
	}
	return false;
}

bool lineSegment::collided(lineSegment v) {
	vec2<float> colPoint = line(*this).collisionPoint(line(v));
	return(pointBelongsTo(colPoint) && v.pointBelongsTo(colPoint));
}