//
// Created by 冯灏 on 5/9/20.
//

#ifndef MESHSIMPLE_MESHSIMPLEQEM_H
#define MESHSIMPLE_MESHSIMPLEQEM_H
//
// Created by 冯灏 on 5/9/20.
//
#include <algorithm>
#include <iostream>
#include <assert.h>
#include <fstream>
#include <climits>
#include <cstring>
#include <limits>
#include <vector>
#include <cstdio>
#include <math.h>
#include <time.h>
#include <cmath>
#include <queue>
#include <list>
#include <set>
#include <map>
#include "./Global.h"
#include "./Vec.h"
#include "./Object_Loader.h"
using namespace std;


priority_queue<Pair, vector<Pair>, greater<Pair> > pairs;

//get the plane
inline void getSurfaceEquation(int o) {
    Vec& a=V[T[o][0]], b=V[T[o][1]], c=V[T[o][2]];
    equaT[o][0] = (b.y - a.y) * (c.z - a.z) - (c.y - a.y) * (b.z - a.z);
    equaT[o][1] = (b.z - a.z) * (c.x - a.x) - (c.z - a.z) * (b.x - a.x);
    equaT[o][2] = (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
    double length = sqrt(SQR(equaT[o][0]) + SQR(equaT[o][1]) + SQR(equaT[o][2]));
    LOOP(i, 3) equaT[o][i] /= length;
    equaT[o][3] = - equaT[o][0] * a.x - equaT[o][1] * a.y - equaT[o][2] * a.z;
}


//calculate cost
inline Pair getCost(int u, int v) {
    //first calculate matrix Q
    double Q[4][4];
    LOOP(i, 4) LOOP(j, 4) Q[i][j] = matrixV[u][i][j] + matrixV[v][i][j];

    // getMidPoint (v1,v2)-->v'
    Vec bestV;
    //this matrix is invertible or not
    double delta = Q[0][0] * Q[1][1] * Q[2][2] - Q[0][0] * Q[1][2] * Q[1][2] - Q[0][1] * Q[0][1] * Q[2][2]
                   + Q[0][1] * Q[1][2] * Q[0][2] + Q[0][2] * Q[0][1] * Q[1][2] - Q[0][2] * Q[1][1] * Q[0][2];
    if (delta > 1e-7) {
        double deltaX = Q[0][3] * Q[1][1] * Q[2][2] - Q[0][3] * Q[1][2] * Q[1][2] - Q[1][3] * Q[0][1] * Q[2][2]
                        + Q[1][3] * Q[1][2] * Q[0][2] + Q[2][3] * Q[0][1] * Q[1][2] - Q[2][3] * Q[1][1] * Q[0][2];
        double deltaY = Q[0][0] * Q[1][3] * Q[2][2] - Q[0][0] * Q[2][3] * Q[1][2] - Q[0][1] * Q[0][3] * Q[2][2]
                        + Q[0][1] * Q[2][3] * Q[0][2] + Q[0][2] * Q[0][3] * Q[1][2] - Q[0][2] * Q[1][3] * Q[0][2];
        double deltaZ = Q[0][0] * Q[1][1] * Q[2][3] - Q[0][0] * Q[1][2] * Q[1][3] - Q[0][1] * Q[0][1] * Q[2][3]
                        + Q[0][1] * Q[1][2] * Q[0][3] + Q[0][2] * Q[0][1] * Q[1][3] - Q[0][2] * Q[1][1] * Q[0][3];
        bestV = Vec(deltaX, deltaY, deltaZ) / (-delta);
    }
    else {
        bestV = (V[u] + V[v]) / 2;
    }
    double tmp[4];
    tmp[0] = bestV.x; tmp[1] = bestV.y; tmp[2] = bestV.z; tmp[3] = 1;
    //deltaV==>cost，Quadratic term
    double deltaV = 0;
    LOOP(i, 4) LOOP(j, 4) deltaV += tmp[j] * Q[j][i] * tmp[i];
    //at the edge of the model, we try not  to change them
    if (verV[u].size() != triV[u].size() || verV[v].size() != triV[v].size())
        deltaV += 9.96;

    return (Pair){u, v, deltaV, bestV, numV};
}

//new triangle
inline int insertT(int o) {
    LOOP(i, 3) T[numT][i] = T[o][i];
    getSurfaceEquation(numT);
    LOOP(j, 4)
        LOOP(k, 4) matrixT[numT][j][k] = equaT[numT][j] * equaT[numT][k];//update Kp
    LOOP(i, 3) triV[T[numT][i]].insert(numT);//update adjacency triangle
    LOOP(i, 3)
        LOOP(j, 4)
            LOOP(k, 4) matrixV[T[numT][i]][j][k] += matrixT[numT][j][k];//update Q
    return numT++;
}

inline void removeV(int o) {
    if (disableV[o]) return;
    disableV[o] = true;
    TRAVERSAL(set<int>, verV[o])
        verV[*it].erase(o);//update adjacency vertex
    rmV++;
}

inline void removeT(int o) {
    if (disableT[o]) return;
    disableT[o] = true;
    LOOP(i, 3)
        LOOP(j, 4)
            LOOP(k, 4) matrixV[T[o][i]][j][k] -= matrixT[o][j][k];//update Q
    LOOP(i, 3)
        triV[T[o][i]].erase(o);
    rmT++;
}

//judge if plane flipped
bool flip(int a, int b, Vec o) {
    //new vertex
    Vec tmp;
    V[numV] = o;
    TRAVERSAL(set<int>, triV[a]) {
        LOOP(i, 3)
            if (T[*it][i] == a && T[*it][(i+1)%3] != b && T[*it][(i+2)%3] != b) {
                //update triangle v1->v'
                T[*it][i] = numV;
                getSurfaceEquation(*it);
                tmp = Vec(equaT[*it][0], equaT[*it][1], equaT[*it][2]);
                T[*it][i] = a;
                getSurfaceEquation(*it);
                if (tmp.dot(Vec(equaT[*it][0], equaT[*it][1], equaT[*it][2])) < 0) return true;
            }
    }
    TRAVERSAL(set<int>, triV[b]) {
        LOOP(i, 3)
            if (T[*it][i] == b && T[*it][(i+1)%3] != a && T[*it][(i+2)%3] != a) {
                //update triangle v2->v'
                T[*it][i] = numV;
                getSurfaceEquation(*it);
                tmp = Vec(equaT[*it][0], equaT[*it][1], equaT[*it][2]);
                T[*it][i] = b;
                getSurfaceEquation(*it);
                if (tmp.dot(Vec(equaT[*it][0], equaT[*it][1], equaT[*it][2])) < 0) return true;
            }
    }
    return false;
}



void initing(){
    //init adjacency triangle
    LOOP(i, numT)
        LOOP(j, 3) triV[T[i][j]].insert(i);

    //init equation
    LOOP(i, numT) getSurfaceEquation(i);

    //init adjacency vertex
    LOOP(i, numT)
        LOOP(j, 3)
            LOOP(k, 2) verV[T[i][j]].insert(T[i][(j+k+1)%3]);//do not insert it self

    //init Kp
    LOOP(i, numT)
        LOOP(j, 4)
            LOOP(k, 4) matrixT[i][j][k] = equaT[i][j] * equaT[i][k];

    //init Q
    LOOP(i, numV)
        TRAVERSAL(set<int>, triV[i])
            LOOP(j, 4)
                LOOP(k, 4) matrixV[i][j][k] += matrixT[*it][j][k];

    //init heap
    LOOP(i, numT)
        LOOP(j, 3) pairs.push(getCost(T[i][j], T[i][(j+1)%3]));//push in heap

    //init tag
    LOOP(i, numV) tag[i] = numV;
}

//iterative edge contraction process
void deleteV() {
    //get min cost pair
    Pair mn = pairs.top(); pairs.pop();
    //when tag[m] is bigger than mn.tag this pair is disable , this pair is updated
    while (disableV[mn.u] || disableV[mn.v] || tag[mn.u] > mn.tag || tag[mn.v] > mn.tag) {
        mn = pairs.top(); pairs.pop();
    }
    if (flip(mn.u, mn.v, mn.bestV)) return; // judge plane flip
    int newV = numV++;
    V[newV] = mn.bestV;
    removeV(mn.u); removeV(mn.v);
    //remove all triangle
    vector<int> needRemoveT;
    TRAVERSAL(set<int>, triV[mn.u])
        needRemoveT.push_back(*it);
    TRAVERSAL(set<int>, triV[mn.v])
        needRemoveT.push_back(*it);
    //delete some update some,first delete ,then insert new triangles to update
    set<int> unionT;
    set_union(triV[mn.u].begin(), triV[mn.u].end(), triV[mn.v].begin(), triV[mn.v].end(),inserter(unionT, unionT.begin()));
    set<int> intersectionT;
    set_intersection(triV[mn.u].begin(), triV[mn.u].end(), triV[mn.v].begin(), triV[mn.v].end(),inserter(intersectionT, intersectionT.begin()));
    vector<int> needUpdateT;
    set_difference(unionT.begin(), unionT.end(), intersectionT.begin(), intersectionT.end(),inserter(needUpdateT, needUpdateT.begin()));
    set<int> needUpdateV;
    TRAVERSAL(vector<int>, needRemoveT)
        removeT(*it);
    vector<int> newT;
    TRAVERSAL(vector<int>, needUpdateT)
        LOOP(i, 3)
            //other vertex
            if (T[*it][i] != mn.u && T[*it][i] != mn.v)
                needUpdateV.insert(T[*it][i]);
                //u or v
            else {
                int tmpV = T[*it][i];
                T[*it][i] = newV;
                newT.push_back(insertT(*it));
                T[*it][i] = tmpV;
            }
    TRAVERSAL(set<int>, needUpdateV)
        tag[*it] = numV;tag[newV] = numV;
    TRAVERSAL(set<int>, needUpdateV) {
        verV[*it].insert(newV);
        verV[newV].insert(*it);
    }
    TRAVERSAL(set<int>, needUpdateV)
        //no need to delete adjacency old pairs because tags are changed means those have been update
        pairs.push(getCost(*it, newV));
    TRAVERSAL(vector<int>, newT)
        //new triangle two vertex except v'
        LOOP(i, 3)
            if (T[*it][i] == newV) {
                pairs.push(getCost(T[*it][(i+1)%3], T[*it][(i+2)%3]));
            }
    TRAVERSAL(set<int>, needUpdateV)
        _TRAVERSAL(set<int>, verV[*it])
            //adjacency triangle change these triangles' adjacency vertexs need update
            if (*_it != newV && needUpdateV.find(*_it) == needUpdateV.end())
                pairs.push(getCost(*it, *_it));
}

void IterativeDelete(double tar){
    int tmpT = numT;
    double target=tar;
    bool sigout[11];LOOP(i,11)sigout[i]=false;

    while (1. * (numT - rmT) / tmpT > target) {
        if((100*(tmpT-numT+rmT)/(int)((1-target)*tmpT))%10==0&&sigout[100*(tmpT-numT+rmT)/(int)((1-target)*tmpT)/10]==false)
            printf("already finish %d percents \n",100*(tmpT-numT+rmT)/(int)((1-target)*tmpT));
        sigout[100*(tmpT-numT+rmT)/(int)((1-target)*tmpT)/10]=true;

        deleteV();
    }
}

#endif //MESHSIMPLE_MESHSIMPLEQEM_H
