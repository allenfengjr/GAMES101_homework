//
// Created by haofeng on 5/1/20.
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
#include "meshsimpleQEM.h"
#include "Delaunay.h"

int main() {

    printf("Please input :  <input obj file>    <output obj file>    <simplify->1  remesh->2>    if simplify  input   <simplification ratio>\n");
    char infile[256];cin>>infile;
    char outfile[256];cin>>outfile;
    int ros;cin>>ros;
    if(ros==1) {
        double target;
        cin >> target;
        clock_t start = clock();
        load(infile);

        printf("Loading from %s successfully.\n", infile);
        printf("Vertex Number = %d\n", numV);
        printf("Triangle Number = %d\n", numT);

        initing();
        IterativeDelete(target);

        printf("Used: %.3lf sec.\n", (double) (clock() - start) / CLOCKS_PER_SEC);
        save(outfile);
    }
    else{
        vector<tetra*> pelements;
        loadDots(infile);
        printf("have dots  %d\n",numD);
        MakeMesh(pelements);
        saveDots(pelements,outfile);
        printf("remash finish\n");
    }
    return 0;
}