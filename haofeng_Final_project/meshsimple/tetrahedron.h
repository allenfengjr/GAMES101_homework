//
// Created by haofeng on 5/8/20.
//
#include "Global.h"
#include "Vec.h"
#include <algorithm>
#ifndef MESHSIMPLE_TETRAHEDRON_H
#define MESHSIMPLE_TETRAHEDRON_H
struct tetra;
struct Surface;
struct Surface{
    Surface(int, int, int, tetra*, tetra*);
    tetra* pneighbor;
    bool IsActive;
    int pnodes[3];
    tetra* pparent;
    bool operator==(const Surface&);
    bool IsRayCross(Vec s, Vec dir);
};

Surface::Surface(int _pnode0, int _pnode1, int _pnode2, tetra* _pparent, tetra* _pneighbor){
    pnodes[0] = _pnode0;
    pnodes[1] = _pnode1;
    pnodes[2] = _pnode2;
    this->pparent = _pparent;
    this->pneighbor = _pneighbor;
    this->IsActive = true;
}

bool Surface::operator==(const Surface& _surface){

    if ((this->pnodes[0] == _surface.pnodes[0] && this->pnodes[1] == _surface.pnodes[2] && this->pnodes[2] == _surface.pnodes[1])
        || (this->pnodes[0] == _surface.pnodes[1] && this->pnodes[1] == _surface.pnodes[0] && this->pnodes[2] == _surface.pnodes[2])
        || (this->pnodes[0] == _surface.pnodes[2] && this->pnodes[1] == _surface.pnodes[1] && this->pnodes[2] == _surface.pnodes[0])) {
        return true;
    }

    return false;
}


bool Surface::IsRayCross(Vec sp, Vec dir) {
    Vec v01 = D[pnodes[1]]-D[pnodes[0]];
    Vec v02 = D[pnodes[2]]-D[pnodes[0]];
    Vec v0g = sp - D[pnodes[0]];
    double det = v01.dot(v02.cross( dir));
    if (det > EPS) {
        double u = v0g .dot(v02.cross( dir)) / det;
        if (-EPS < u && u < 1.0 + EPS) {
            double v = v01.dot(v0g.cross( dir)) / det;
            if (-EPS < v && u + v < 1.0 + EPS) {
                double t = v01.dot (v02.cross(v0g)) / det;
                if (t > -EPS && t < 1.0 - EPS) {
                    return true;
                }
            }
        }
    }
    return false;
}

struct tetra{
    bool IsActive;
    Surface*  psurfaces[4];
    int pnodes[4];
    Vec scenter;
    double sround;
    Vec gcenter;
    double volume;
    double aspect;
    tetra* GetLocateId(int);
    bool IsInSphere(int);
    Surface* GetAdjacentSurface(tetra*);
    tetra(int _pnode0, int _pnode1, int _pnode2, int _pnode3);
};

tetra::tetra(int _pnode0, int _pnode1, int _pnode2, int _pnode3){
    this->IsActive = true;
    //Set nodes
    this->pnodes[0] = _pnode0;
    this->pnodes[1] = _pnode1;
    this->pnodes[2] = _pnode2;
    this->pnodes[3] = _pnode3;

    //Set surfaces
    this->psurfaces[0] = new Surface(_pnode1, _pnode3, _pnode2, this, nullptr);
    this->psurfaces[1] = new Surface(_pnode0, _pnode2, _pnode3, this, nullptr);
    this->psurfaces[2] = new Surface(_pnode0, _pnode3, _pnode1, this, nullptr);
    this->psurfaces[3] = new Surface(_pnode0, _pnode1, _pnode2, this, nullptr);

    //Get center and radius of external sphere
    Vec v0 = D[_pnode1] - D[_pnode0];
    Vec v1 = D[_pnode2] - D[_pnode0];
    Vec v2 = D[_pnode3] - D[_pnode0];

    Vec ABC = Vec(0.5*D[_pnode1].dot(D[_pnode1]) - D[_pnode0] .dot(D[_pnode0]),
                  0.5*D[_pnode2].dot(D[_pnode2])- D[_pnode0].dot(D[_pnode0]),
                  0.5*D[_pnode3].dot(D[_pnode3]) - D[_pnode0].dot(D[_pnode0]));

    double detP = v0.dot(v1.cross( v2));
    Vec P0 = v1.cross(v2);
    Vec P1 = v2.cross(v0);
    Vec P2 = v0.cross(v1);

    this->scenter = Vec((ABC.x * P0.x + ABC.y * P1.x + ABC.z * P2.x) / detP,
                        (ABC.x * P0.y + ABC.y * P1.y + ABC.z * P2.y) / detP,
                        (ABC.x * P0.z + ABC.y * P1.z + ABC.z * P2.z) / detP);
    this->sround = (this->scenter - D[_pnode0]).len();

    //Get center of gravity
    this->gcenter = (D[_pnode0] + D[_pnode1] + D[_pnode2] + D[_pnode3]) / 4.0;

    //Get volume
    this->volume = ((D[_pnode1] - D[_pnode0]).cross(D[_pnode2] - D[_pnode0])).dot(D[_pnode3] - D[_pnode0]);

    //Get aspect ratio
    this->aspect = this->volume / pow(this->sround, 3.0) / ARTETRAHEDRON;

}

tetra* tetra::GetLocateId(int _pnode){
        for (auto sur :this->psurfaces) {
            if (sur->IsRayCross(this->gcenter, this->gcenter - D[_pnode])) {
                return sur->pneighbor;
            }
        }
    return this;
}
bool tetra::IsInSphere(int _pnode){
    if (sround + EPS > (this->scenter - D[_pnode]).len()) {
        return true;
    }
    return false;
}
Surface* tetra::GetAdjacentSurface(tetra* _pelement){
    std::cout<<"getADsurface"<<std::endl;
    LOOP(i,4)
    std::cout<<this->pnodes[i]<<" ";
    std::cout<<endl;
    LOOP(i,4) {
        if(psurfaces[i]== nullptr){
            std::cout<<"NULLERROR"<<std::endl;
        }
        else if (psurfaces[i]->pneighbor == _pelement) {
            return psurfaces[i];
        }
    }

    return nullptr;
}


#endif //MESHSIMPLE_TETRAHEDRON_H
