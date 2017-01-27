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
#include <set>

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

	void moveTo(vec2<float> v) {
		moveTo(v.x, v.y);
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

		calculatePoints();

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

		std::vector<vec2<float>> poi;
		lineSegment segments[4];
		for (int i = 0; i < 4; i++) {
			segments[i] = lineSegment(points[i], points[(i+1)%4]);
		}
		std::cout << "==================" << std::endl;
		for (int i = 0; i < 4; i++) {
			if (ray.segmentCollide(segments[i])) {
				if (ray.collisionPoint(segments[i]).x > a.x) {
					if (!std::any_of(poi.begin(), poi.end(), [s = ray.collisionPoint(segments[i])](vec2<float> i) { return std::abs(i.x - s.x) <= 0.01; }))
					poi.push_back(ray.collisionPoint(segments[i]));
				}
			}
		}

		if ((int)(poi.size()) % 2 == 0) {
			return false;
		}
		else {
			return true;
		}
	}

};

class polygon : public object {

private:
	size_t vertices;
	std::vector<vec2<float>> vertex;
	bool simple;

	polygon monotoneTriangulate() {
		std::vector<vec2<float>> sortedVertices = vertex;
		std::sort(vertex.begin(), vertex.end(), [](vec2<float> i, vec2<float> j) { return (i.x < j.x); });
		std::vector<lineSegment> polygonEdges = edges();
		std::vector<lineSegment> diagonalVector;

		std::vector<vec2<float>> stack;
		stack.push_back(sortedVertices[0]);
		stack.push_back(sortedVertices[1]);

		for (int i = 0; i < (int)sortedVertices.size(); i++) {
			//Checking to see if v[i] is the the same chain as top(stack)
			int indexOfV = std::distance(vertex.begin(), std::find(vertex.begin(), vertex.end(), sortedVertices[i]));
			if (vertex[indexOfV - 1] == stack.back() || vertex[indexOfV + 1] == stack.back()) {
				stack.pop_back();
				while (true) {
					lineSegment diagonal(sortedVertices[i],stack.back());
					//Need to check for legal diagonals
					//If the centre of the diagonal is outside the shape or the diagonal collides with a line segment of the polygon, illegal
					if (!collidePoint(vec2<float>((sortedVertices[i].x + stack.back().x) / 2, (sortedVertices[i].y + stack.back().y) / 2))) {
						break;
					}
					bool legal = true;
					//Loop through edges to see if the diagonal intersects any of them, not including the end points of the diagonal
					for (int a = 0; a < polygonEdges.size(); a++) {
						if (diagonal.collided(polygonEdges[a])) {
							legal = false;
						}
					}
					if (!legal) {
						break;
					}
					else {
						diagonalVector.push_back(diagonal);
					}
				}
			}
			//If not in the same chain
			else {
				vec2<float> vtop = stack.back();
				for (int a = 0; a < stack.size(); a++) {
					diagonalVector.push_back(lineSegment(sortedVertices[i],stack.back()));
					stack.pop_back();
				}
				stack.push_back(vtop);
				stack.push_back(sortedVertices[i]);
			}
		}

		//Diagonals all set, need to create triangles


	}

	std::vector<lineSegment> edges() {
		std::vector<lineSegment> segs;
		for (int i = 0; i < vertices; i++) {
			segs.push_back(lineSegment(vertex[i], vertex[(i + 1) % vertices]));
		}
		return segs;
	}

public:
	
	polygon::polygon(vec2<float> v, float m, std::vector<vec2<float>> container)
		//Two giant lines are functions that return the minimum x value, and the minimum y value to use for the position variable
		//i.e. the top left corner of the polygon
		: object(vec2<float>((*std::min_element(container.begin(), container.end(), [](vec2<float> i, vec2<float> j) { return i.x < j.x; })).x, 
		(*std::min_element(container.begin(), container.end(), [](vec2<float> i, vec2<float> j) { return i.y < j.y; })).y), 
			v, m) {
		std::vector<vec2<float>>::iterator current;
		for (current = container.begin(); current != container.end(); current++)
		{
			vertex.push_back(*current);
		}

		vertices = vertex.size();
		objType = "polygon";
		simple = true;

		lineSegment *segs = new lineSegment[vertices];

		for (int i = 0; i < (int)vertices; i++) {
			segs[i] = lineSegment(vertex[i],vertex[(i+1)%vertices]);
			for (int a = 0; a < i; a++) {
				if (segs[i].collided(segs[a])) {
					simple = false;
				}
			}
			if (!simple) {
				vertex.clear();
				break;
			}
		}

	}

	void draw(SDL_Renderer *rend) {
		SDL_Point *drawpts = new SDL_Point[vertices+1];
		for (int i = 0; i < 4; i++) {
			drawpts[i] = { (int)(vertex[i].x + 0.5), (int)(vertex[i].y + 0.5) };
		}
		drawpts[vertices] = drawpts[0];
		SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
		SDL_RenderDrawLines(rend, drawpts, 5);
	}

	bool collidePoint(vec2<float> a) {

		//Need to handle the case of the ray passing through a corner
		line ray(vec2<float>(1, 0), a);

		std::vector<vec2<float>> poi;
		std::vector<lineSegment> segments = edges();

		for (int i = 0; i < vertices; i++) {
			if (ray.segmentCollide(segments[i])) {
				if (ray.collisionPoint(segments[i]).x > a.x) {
					if (std::find(poi.begin(), poi.end(), ray.collisionPoint(segments[i])) == poi.end())
						poi.push_back(ray.collisionPoint(segments[i]));
				}
			}
		}

		if ((int)(poi.size()) % 2 == 0) {
			return false;
		}
		else {
			return true;
		}
	}

	int getVertices() {
		return (int)vertices;
	}

	bool getSimple() {
		return (bool)simple;
	}

};