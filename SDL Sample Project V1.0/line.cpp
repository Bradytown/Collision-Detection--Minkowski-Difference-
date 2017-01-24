#include "line.h"
#include "lineSegment.h"

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
	if ((vec2<float>::dot(direction, a.begin - point) > 0
		&& vec2<float>::dot(direction, a.end - point) > 0) ||
		(vec2<float>::dot(direction, a.begin - point) < 0
			&& vec2<float>::dot(direction, a.end - point) < 0)) {
		return false;
	}
	if (collisionPoint(line(a)).x == point.x && collisionPoint(line(a)).y == point.y && !(a.pointBelongsTo(point))) {
		return false;
	}
	return true;
}

vec2<float> line::collisionPoint(lineSegment a) {
	return vec2<float>
		(point + direction *
			(point.cross((a.begin - point), (a.end - a.begin))
				/ (direction.cross(direction, a.end - a.begin))));
}

vec2<float> line::collisionPoint(line a) {
	float t;
	t = -((direction.x*(point.y+a.point.y))/(direction.y*(a.direction.x - a.direction.y/direction.y)));
	return a.direction*t + a.point;
}

void line::draw(SDL_Renderer *rend) {
	//THIS IS INCOMPLETE
}