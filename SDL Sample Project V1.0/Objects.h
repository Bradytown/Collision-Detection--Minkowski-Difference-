#pragma once
#include "vec2.h"
#include <vector>
#include <iterator>
#include "matrix.h"
#include <math.h>
#include <iostream>
#include <SDL2_gfxPrimitives.h>
#include "line.h"
#include "lineSegment.h"

class object {

public:
	vec2<float> pos;
	vec2<float> vel;
	float invMass;
	std::string objType;

	object::object(vec2<float> p, vec2<float> v, float m) {

		pos = p;
		vel = v;
		objType = "object";

		if (m == 0) {
			invMass = 0;
		}
		else {
			invMass = 1 / m;
		}

	}

};

class circle : public object {
public:

	float rad;

	circle::circle(vec2<float> p, vec2<float> v, float m, float r) : object(p, v, m) {
		rad = r;
		objType = "circle";
	}

	void draw(SDL_Renderer *rend) {
		filledCircleColor(rend, pos.x, pos.y, rad, 0xFF0000FF);
	}

};

class AABB : public object {
public:
	float w;
	float h;

	float halfw;
	float halfh;

	AABB::AABB(vec2<float> p, vec2<float> v, float m, float width, float height) : object(p, v, m) {
		w = width;
		h = height;

		halfw = w / 2;
		halfh = h / 2;

		objType = "AABB";
	}

	bool collide(circle a) {
		//find closest corner point
		vec2<float> closestPt;
		if (a.pos.x > pos.x) {
			if (a.pos.y > pos.y) {
				//top right
				closestPt.x = pos.x + halfw;
				closestPt.y = pos.y + halfh;
			}
			else {
				//bottom right
				closestPt.x = pos.x + halfw;
				closestPt.y = pos.y - halfh;
			}
		}
		else {
			if (a.pos.y > pos.y) {
				//top left
				closestPt.x = pos.x - halfw;
				closestPt.y = pos.y + halfh;
			}
			else {
				//bottom left
				closestPt.x = pos.x - halfw;
				closestPt.y = pos.y - halfh;
			}
		}

		if ((pow(a.pos.x - closestPt.x, 2) + pow(a.pos.y - closestPt.y, 2)) < pow(a.rad, 2)) {
			return true;
		}
	}

};

class OBB : public AABB {

public:
	transformationMatrix rotation;
	vec2<float> points[4];

	OBB::OBB(vec2<float> p, vec2<float> v, float m, float width, float height, float r) : AABB(p, v, m, width, height) {
		rotation.u.x = cos(r);
		rotation.u.y = -sin(r);
		rotation.v.x = sin(r);
		rotation.v.y = cos(r);
		pos = p;
		rotation.pos = p;
		objType = "OBB";
	}

	void moveTo(float x, float y) {
		pos.x = x;
		pos.y = y;
		rotation.pos = pos;
	}

	void calculatePoints() {
		//find points in object space
		//bottom left
		points[0].x = -halfw;
		points[0].y = -halfh;
		//bottom right
		points[1].x = halfw;
		points[1].y = -halfh;
		//top right
		points[2].x = halfw;
		points[2].y = halfh;
		//top left
		points[3].x = -halfw;
		points[3].y = halfh;

		for (int i = 0; i < 4; i++) {
			points[i] = rotation.transformFrom(points[i]);
		}
	}

	void draw(SDL_Renderer *rend) {
		SDL_Point drawpts[5];
		calculatePoints();
		for (int i = 0; i < 4; i++) {
			drawpts[i] = { (int)(points[i].x + 0.5), (int)(points[i].y + 0.5) };
		}
		drawpts[4] = drawpts[0];
		SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
		SDL_RenderDrawLines(rend, drawpts, 5);
	}

	bool collide(circle a) {

		if (collidePoint(a.pos)) {
			std::cout << "Inner Collision" << std::endl;
		}

		return (collidePoint(a.pos) ||
			lineSegment(points[0], points[1]).distanceToPoint(a.pos) <= a.rad ||
			lineSegment(points[1], points[2]).distanceToPoint(a.pos) <= a.rad ||
			lineSegment(points[2], points[3]).distanceToPoint(a.pos) <= a.rad ||
			lineSegment(points[3], points[0]).distanceToPoint(a.pos) <= a.rad);
	}
	bool collidePoint(vec2<float> a) {

		//Need to handle the case of the ray passing through a corner
		line ray(vec2<float>(1, 0), a);
		calculatePoints();

		int i = 0;

		std::vector<vec2<float>> pointsOfIntersection;


		if (ray.segmentCollide(lineSegment(points[0], points[1]))) {
			if (ray.collisionPoint(lineSegment(points[0], points[1])).x > pos.x) {
				pointsOfIntersection.push_back(ray.collisionPoint(lineSegment(points[0], points[1])));
				i++;
			}
		}
		if (ray.segmentCollide(lineSegment(points[1], points[2]))) {
			if (ray.collisionPoint(lineSegment(points[1], points[2])).x > pos.x 
				&& std::find(pointsOfIntersection.begin(),pointsOfIntersection.end(), ray.collisionPoint(lineSegment(points[1], points[2]))) != pointsOfIntersection.end()) {
				i++;
			}
		}
		if (ray.segmentCollide(lineSegment(points[2], points[3]))) {
			if (ray.collisionPoint(lineSegment(points[2], points[3])).x > pos.x
				&& std::find(pointsOfIntersection.begin(), pointsOfIntersection.end(), ray.collisionPoint(lineSegment(points[1], points[2]))) != pointsOfIntersection.end()) {
				i++;
			}
		}
		if (ray.segmentCollide(lineSegment(points[3], points[0]))) {
			if (ray.collisionPoint(lineSegment(points[3], points[0])).x > pos.x
				&& std::find(pointsOfIntersection.begin(), pointsOfIntersection.end(), ray.collisionPoint(lineSegment(points[1], points[2]))) != pointsOfIntersection.end()) {
				i++;
			}
		}

		if (i % 2 == 0) {
			return false;
		}
		else {
			return true;
		}
	}

};

class polygon : public object {

public:
	int vertices;
	std::vector<vec2<float>> vertex;

	polygon::polygon(vec2<float> p, vec2<float> v, float m, std::vector<vec2<float>> container) : object(p, v, m) {
		//insert iterator here
		std::vector<vec2<float>>::iterator current;
		for (current = container.begin(); current != container.end(); current++)
		{
			vertex.push_back(*current);
		}
		vertices = vertex.size();
		objType = "polygon";
	}
};