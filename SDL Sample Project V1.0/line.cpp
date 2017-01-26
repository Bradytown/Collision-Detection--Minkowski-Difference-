#include "line.h"
#include "lineSegment.h"
#include <iostream>

line::line() {
	direction.x = 1;
	direction.y = 0;
	point.x = 0;
	point.y = 0;
}

line::line(vec2<float> d, vec2<float> p) {
	direction = d;
	point = p;
}

line::line(lineSegment a) {
	point = a.begin;
	direction = a.end - a.begin;
}

bool line::segmentCollide(lineSegment a) {
	//Theoretically works
	//Point belongs to is solid, so is collisionPoint
	if (a.pointBelongsTo(collisionPoint(line(a)))) {
		return true;
	}
	return false;
}

vec2<float> line::collisionPoint(lineSegment a) {
	return vec2<float>
		(point + direction *
			(point.cross((a.begin - point), (a.end - a.begin))
				/ (direction.cross(direction, a.end - a.begin))));
}

vec2<float> line::collisionPoint(line a) {
	//Works perfectly if lines aren't parallel
	float t;
	float denominator = (direction.x*a.direction.y - direction.y*a.direction.x);
	t = (direction.y*(a.point.x-point.x)-direction.x*(a.point.y-point.y))/denominator;
	return a.direction*t + a.point;
}

void line::draw(SDL_Renderer *rend){
	//Still have no clue
}