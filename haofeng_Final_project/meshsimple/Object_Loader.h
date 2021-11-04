//
// Created by haofeng on 5/1/20.
//
#include "Global.h"
#include "Vec.h"
#include "tetrahedron.h"
#ifndef MESHSIMPLE_OBJECT_LOADER_H
#define MESHSIMPLE_OBJECT_LOADER_H
void load(const char* filename) {
    numV = numT = 0;
    FILE* f = fopen(filename, "r");
    char buf[256];
    while(fscanf(f, "%s", buf) != EOF) {
        switch (buf[0]) {
            //thanks to TT teaching me fscanf and fgets
            case '#':
                fgets(buf, sizeof(buf), f);
                break;
            case 'v':
                fscanf(f, "%lf %lf %lf", &V[numV].x, &V[numV].y, &V[numV].z);
                numV++;
                break;
            case 'f':
                fscanf(f, "%d %d %d", &T[numT][0], &T[numT][1], &T[numT][2]);
                LOOP(i, 3) T[numT][i]--;//index,V[] start from 0 so in T V=V-1
                numT++;
                break;
            default:
                fgets(buf, sizeof(buf), f);
        }
    }
    fclose(f);
}

void save(const char* filename) {
    FILE* f = fopen(filename, "w");

    fprintf(f, "# %d vertices %d triangles\n", numV-rmV, numT-rmT);
    int total = 0;
    //only still exists V and T write in *.obj
    LOOP(i, numV) if (disableV[i] == false) {
            total++;
            fprintf(f, "v %f %f %f\n", V[i].x, V[i].y, V[i].z);
            newID[i] = total;
        }
    LOOP(i, numT) if (disableT[i] == false)
            fprintf(f, "f %d %d %d\n", newID[T[i][0]], newID[T[i][1]], newID[T[i][2]]);

    fclose(f);
    printf("Vertex Number = %d\nTriangle Number = %d\n", numV-rmV, numT-rmT);
    printf("Writing to %s successfully\n", filename);
}

void loadDots(const char * filename){
    numV = numT = 0;
    FILE* f = fopen(filename, "r");
    char buf[256];
    while(fscanf(f, "%s", buf) != EOF) {
        switch (buf[0]) {
            //thanks to TT teaching me fscanf and fgets
            case '#':
                fgets(buf, sizeof(buf), f);
                break;
            case 'v':
                fscanf(f, "%lf %lf %lf", &D[numD].x, &D[numD].y, &D[numD].z);
                numD++;
                break;
            default:
                fgets(buf, sizeof(buf), f);
                fclose(f);
                return;
        }
    }
    fclose(f);
}
void saveDots(std::vector<tetra*>_pelements, const char * filename) {
    FILE* f = fopen(filename, "w");
    int total=0;
    LOOP(i,numD)
        if (disableV[i] == false) {
        total++;
        fprintf(f, "v %f %f %f\n", V[i].x, V[i].y, V[i].z);
        newID[i] = total;
    for (auto pelement : _pelements) {
        LOOP(j, 4) {
            if(pelement->psurfaces[j]->IsActive) {
                fprintf(f, "f %f %f %f\n",
                        newID[pelement->psurfaces[j]->pnodes[0]],
                        newID[pelement->psurfaces[j]->pnodes[1]],
                        newID[pelement->psurfaces[j]->pnodes[2]]);
            }
        }
    }
    fclose(f);
}


}
#endif //MESHSIMPLE_OBJECT_LOADER_H
