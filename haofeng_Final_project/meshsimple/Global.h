//
// Created by haofeng on 5/1/20.
//
#include "Vec.h"
#include <set>
#include <algorithm>
#include <cmath>
using namespace std;
#pragma once

#ifndef MESHSIMPLE_GLOBAL_H
#define MESHSIMPLE_GLOBAL_H
#define maxs_V 5000000
#define maxs_T 5000000

#define LOOP(i, n) for(int i = 0; i < n; i++)
#define TRAVERSAL(ty1, nm) for(ty1::iterator it = nm.begin(); it != nm.end(); ++it)
#define _TRAVERSAL(ty2, nm) for(ty2::iterator _it = nm.begin(); _it != nm.end(); ++_it)
#define SQR(x) ((x)*(x))

#define ALPHA 4.0                           //Size parameter of supertetrahedron
#define EPS 1.0e-15                         //Minimum value epsilon
#define ARTETRAHEDRON 8.0*sqrt(3.0)/27.0	//Aspect ratio of reguler tetrahedron



int numV = 0, numT = 0,numD=0;
Vec V[maxs_V];//vector point
Vec D[1];//DotCloud
int T[maxs_T][3];//triangles
int rmV = 0, rmT = 0;

std::set<int> triV[maxs_V];//adjacency triangle
std::set<int> verV[maxs_V];//adjacency vertex
double equaT[maxs_T][4];
double matrixT[maxs_T][4][4];
double matrixV[maxs_V][4][4];
bool disableV[maxs_V];
bool disableT[maxs_T];
int tag[maxs_V];//the number of the model when this vertex is insert


int newID[maxs_V];

#endif //MESHSIMPLE_GLOBAL_H
