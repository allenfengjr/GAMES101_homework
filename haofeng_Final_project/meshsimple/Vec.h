//
// Created by haofeng on 5/1/20.
//
#include <vector>
#include <algorithm>
#include <queue>
#ifndef MESHSIMPLE_VEC_H
#define MESHSIMPLE_VEC_H
struct Vec {
    double x, y, z;
    Vec(double _x = 0, double _y = 0, double _z = 0) : x(_x), y(_y), z(_z) {}
    Vec operator+(const Vec& b) const { return Vec(x + b.x, y + b.y, z + b.z); }
    Vec &operator+=(const Vec &b) { x += b.x, y += b.y, z += b.z; return *this; }
    Vec operator-(const Vec& b) const { return Vec(x - b.x, y - b.y, z - b.z); }
    Vec &operator-=(const Vec &b) { x -= b.x, y -= b.y, z -= b.z; return *this; }
    Vec operator*(double b) const { return Vec(x * b, y * b, z * b); }
    Vec operator/(double b) const { return Vec(x / b, y / b, z / b); }
    Vec &operator/=(double b) { x /= b, y /= b, z /= b; return *this; }
    Vec mult(const Vec &b) const { return Vec(x * b.x, y * b.y, z * b.z); }
    double dot(const Vec &b) const { return x * b.x + y * b.y + z * b.z; }
    Vec cross(const Vec &b) const { return Vec(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x); }
    Vec& norm() { return *this = *this * (1 / sqrt(x*x + y*y + z*z)); }
    double len() const { return sqrt(x*x + y*y + z*z); }
    double max() const { return (x>y && x>z) ? x : (y>z ? y : z); }
    double min() const { return (x<y && x<z) ? x : (y<z ? y : z); }
    void print() const { printf("%.12lf %.12lf %.12lf\n", x, y, z); }
};

struct Pair { int u, v; double cost; Vec bestV; int tag; };
bool operator>(const Pair& a, const Pair& b) { return a.cost > b.cost; }

#endif //MESHSIMPLE_VEC_H
