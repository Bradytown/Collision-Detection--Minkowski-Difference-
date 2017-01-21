#pragma once
#include "vec2.h"

class matrix {
public:
	vec2<float> u;
	vec2<float> v;

	matrix operator*(matrix s) {
		vec2<float> a;
		vec2<float> b;

		a.x = u.dot(u, s.u);
		a.y = u.dot(u, s.v);
		b.x = v.dot(v, s.u);
		b.y = v.dot(v, s.v);

		return matrix(a, b);
	}

	vec2<float> operator*(vec2<float> a) {
		return vec2<float>(u.dot(u, a), v.dot(v, a));
	}

	matrix::matrix() {
		u.x = 1;
		u.y = 0;
		v.x = 0;
		v.y = 1;
	}

	matrix::matrix(vec2<float> a, vec2<float> b) {
		u = a / a.length();
		v = b / b.length();
	}


	matrix transpose() {
		vec2<float> a;
		vec2<float> b;

		a.x = u.x;
		a.y = v.x;
		b.x = u.y;
		b.y = v.y;

		return matrix(a, b);
	}

};

class rotationMatrix : public matrix {
public:

	rotationMatrix::rotationMatrix(vec2<float> a, vec2<float> b) {
		u = a;
		v = b;
	}

	rotationMatrix::rotationMatrix(matrix a) {
		u = a.u;
		v = a.v;
	}

	vec2<float> transformIntoSpaceOf(vec2<float> a) {
		float vx;
		float vy;
		vx = a.dot(a, u);
		vy = a.dot(a, v);

		return vec2<float>(vx, vy);
	}

	vec2<float> transformFrom(vec2<float> a) {
		return vec2<float>(u*a.x + v*a.y);
	}
};

class transformationMatrix : public matrix {
public:
	vec2<float> pos;

	transformationMatrix::transformationMatrix() : matrix() {
		pos.x = 0;
		pos.y = 0;
	}

	transformationMatrix::transformationMatrix(vec2<float> a, vec2<float> b, vec2<float> c) : matrix(a, b) {
		pos = c;
	}

	transformationMatrix::transformationMatrix(matrix a, vec2<float> c) {
		u = a.u;
		v = a.v;
		pos = c;
	}

	transformationMatrix invert() {
		return transformationMatrix(transpose(), transpose()*(pos)*(-1));
	}

	vec2<float> transformIntoSpaceOf(vec2<float> p) {
		return (rotationMatrix(u, v).transpose())*(p - pos);
	}

	vec2<float> transformFrom(vec2<float> p) {
		return rotationMatrix(u, v)*p + pos;
	}

};