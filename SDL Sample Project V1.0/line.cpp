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
	if ((direction.dot(direction, a.begin - point) > 0
		&& direction.dot(direction, a.end - point) > 0) ||
		(direction.dot(direction, a.begin - point) < 0
			&& direction.dot(direction, a.end - point) < 0)) {
		return false;
	}
	else {
		return true;
	}
}

vec2<float> line::collisionPoint(lineSegment a) {
	return vec2<float>
		(point + direction *
			(point.cross((a.begin - point), (a.end - a.begin))
				/ (direction.cross(direction, a.end - a.begin))));
}