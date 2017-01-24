#include "lineSegment.h"

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

	vec2<float> lineSegmentDirection(end - begin);

	//check if line perpindicular to line segment originating from point intersects segment
	//if so, that distance is shortest
	line perp(vec2<float>(-lineSegmentDirection.y, lineSegmentDirection.x), p);

	if (perp.segmentCollide(*this)) {
		vec2<float> collisionPoint = perp.collisionPoint(*this);
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
	if (vec2<float>::dot(line(*this).direction, vec2<float>(a.point.y - v.y, v.x - a.point.x)) == 0 && v.x >= std::min(begin.x, end.x) && v.x <= std::max(begin.x, end.x) && v.y >= std::min(begin.y, end.y) && v.x <= std::max(begin.y, end.y)) {
		return true;
	}
	return false;
}
