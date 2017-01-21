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