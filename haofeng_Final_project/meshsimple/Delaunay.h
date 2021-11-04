//
// Created by haofeng on 5/8/20.
//

#pragma once
#define _USE_MAdoubleH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>
#include "Global.h"

#include "Vec.h"
#include "tetrahedron.h"
	//	Make supertetrahedron
	void MakeSupertetrahedron( std::vector<tetra*>& _pelements, double _xmax, double _ymax, double _zmax) {
		std::cout << "Make supertetraedron\n";

		//Make nodes of supertetrahedron
		Vec nsts[8];int nstsnum[8];
		nsts[0] = Vec (0, 0, 0);
		nsts[1] = Vec(_xmax, 0, 0);
		nsts[2] = Vec(_xmax, _ymax, 0);
		nsts[3] = Vec(0, _ymax, 0);
		nsts[4] = Vec(0, 0, _zmax);
		nsts[5] = Vec(_xmax, 0, _zmax);
		nsts[6] = Vec(_xmax, _ymax, _zmax);
		nsts[7] = Vec(0, _ymax, _zmax);
		LOOP(i,8) {
		    disableV[numD]=true;
		    nstsnum[i]=numD;
            D[numD++] = nsts[i];
        }

        //Make elements of supertetrahedron
		_pelements.push_back(new tetra(nstsnum[1], nstsnum[3], nstsnum[0], nstsnum[7]));
		_pelements.push_back(new tetra(nstsnum[2], nstsnum[1], nstsnum[6], nstsnum[7]));
		_pelements.push_back(new tetra(nstsnum[2], nstsnum[3], nstsnum[1], nstsnum[7]));
		_pelements.push_back(new tetra(nstsnum[1], nstsnum[5], nstsnum[6], nstsnum[7]));
		_pelements.push_back(new tetra(nstsnum[1], nstsnum[0], nstsnum[5], nstsnum[7]));
		_pelements.push_back(new tetra(nstsnum[4], nstsnum[5], nstsnum[0], nstsnum[7]));

		//Make connection of supertetrahedron
		for (auto& pelement : _pelements) {
		    LOOP(i,4){
				if (pelement->psurfaces[i]->pneighbor == nullptr) {
					for (auto& pelement2 : _pelements) {
					    LOOP(j,4){
							if (*pelement->psurfaces[i] == *pelement->psurfaces[j]) {
								pelement->psurfaces[i]->pneighbor = pelement2;
								pelement->psurfaces[j]->pneighbor = pelement;
								break;
							}
						}
					}
				}
			}
		}
	}


//	Mesh local
void MeshLocal(int _pnode, tetra* _pethis, std::vector<tetra*>& _pelements) {
    std::vector<tetra*> stack, substack;
    std::vector<Surface*> sstack;

	//Get elements which node is in
	substack.push_back(_pethis);
	while (substack.size()) {
	    //substack tetra is active
	    //stack tetra is not active
	    tetra* pend = *(substack.end() - 1);
	    substack.pop_back();
	    if (pend->IsActive) {
	        stack.push_back(pend);
	        pend->IsActive = false;
	        LOOP(i,4){
	            tetra* pneighbor = pend->psurfaces[i]->pneighbor;
	            //if in the neighbor tetra ,push in substack
	            if (pneighbor != nullptr && pneighbor->IsInSphere(_pnode)) {
	                substack.push_back(pneighbor);
	            } else {
	                //this surface not a part of inside tetra this surface
	                sstack.push_back(pend->psurfaces[i]);
	            }
	        }
	    }
	}
	//sstack may update
	//Modify crevice of polyhedron
	bool is_anysurface_invalid = true;
	while (is_anysurface_invalid) {
	    is_anysurface_invalid = false;
	    LOOP(i,sstack.size()){
	        if (sstack[i]->IsActive) {
	            tetra D = tetra(sstack[i]->pnodes[0], sstack[i]->pnodes[1], sstack[i]->pnodes[2], _pnode);
	            //if there are crevices
	            if (D.volume < EPS) {
	                tetra* peadd = sstack[i]->pneighbor;
	                //if able to add elements
	                if (peadd != nullptr) {
	                    if (peadd->IsActive) {
	                        is_anysurface_invalid = true;
	                        peadd->IsActive = false;
	                        stack.push_back(peadd);
	                        //make surfaces isactive false
	                        for (auto& psurface : peadd->psurfaces) {
	                            tetra* pneighbor = psurface->pneighbor;
	                            if (pneighbor != nullptr && !pneighbor->IsActive) {
	                                pneighbor->GetAdjacentSurface(peadd)->IsActive = false;
	                            } else {
	                                sstack.push_back(psurface);
	                            }
	                        }
	                        break;
	                    }
	                } else if (fabs(D.volume) < EPS) {
	                    sstack[i]->IsActive = false;
	                }
	            }
	        }
	    }
	}

	//Make new elements
	std::vector<tetra*> penew;
	for (auto& psurface : sstack) {
	    if (psurface->IsActive) {
	        tetra* tmp = new tetra(psurface->pnodes[0], psurface->pnodes[1], psurface->pnodes[2], _pnode);
	        tmp->psurfaces[3]->pneighbor = psurface->pneighbor;
	        if (psurface->pneighbor != nullptr) {
	            psurface->pneighbor->GetAdjacentSurface(psurface->pparent)->pneighbor = tmp;
	        }
	        penew.push_back(tmp);
	        _pelements.push_back(tmp);
	    }
	}
		//Make connection of new elements
		for (auto& pelement : penew) {
			for (auto& psurface : pelement->psurfaces) {
				OUdouble:
				if (psurface->pneighbor == nullptr) {
					for (auto& pelement2 : penew) {
						for (auto& psurface2 : pelement2->psurfaces) {
							if (*psurface == *psurface2) {

								//if invalid element is made
								if (psurface2->pneighbor != nullptr) {
									std::cout << "!!";
								}

								psurface->pneighbor = pelement2;
								psurface2->pneighbor = pelement;
								goto OUdouble;
							}
						}
					}
				}
			}
		}

		//Delete needless elements in stack
		for (auto pelement = _pelements.begin(); pelement != _pelements.end(); ) {
			if (!(*pelement)->IsActive) {
				delete *pelement;
				pelement = _pelements.erase(pelement);
			} else {
				++pelement;
			}
		}
	}


//	Make rough mesh
void MakeRoughMesh(std::vector<tetra*>& _pelements) {
		std::cout << "Make rough mesh\n";
        for (int pe=0;pe<_pelements.size();pe++) {
            if(_pelements[pe] != nullptr)std::cout<<_pelements[pe]<<std::endl;
        }
		tetra* pethis = _pelements[0];
		LOOP(i,numD){
			if (disableV[i]) {
				int count = 0;
				while (1) {
					tetra* penext = pethis->GetLocateId(i);
					//if node is in the element
					//keep looking
					if (penext == pethis) {
						MeshLocal(i, pethis, _pelements);
						pethis = *(_pelements.end() - 1);
						break;
					} else {
						pethis = penext;
					}
				}
			}
		}
	}


//	Delete supertetrahedron
void DeleteSupertetrahedron(std::vector<tetra*>& _pelements) {
		std::cout << "Delete supertetraedron\n";
        std::vector<tetra*>::iterator it=_pelements.begin();
		while(it!=_pelements.end()){
		    //=-1 means disable=true
			if (disableV[(*it)->pnodes[0]]==true
				|| (*it)->pnodes[1]==true
				|| (*it)->pnodes[2]==true
				|| (*it)->pnodes[3]==true) {
				for (auto& psurface : (*it)->psurfaces) {
					if (psurface->pneighbor != nullptr) {
						psurface->pneighbor->GetAdjacentSurface(psurface->pparent)->pneighbor = nullptr;
					}
				}
				//move to next
				delete *it;
				it = _pelements.erase(it);
			} else {
				++it;
			}
		}
	}


//	Make Mesh
void MakeMesh( std::vector<tetra*>& _pelements){
    //Get region which nodes exist
    double xmax =-1e5,xmin=1e5,ymax =-1e5,ymin =1e5,zmax =-1e5,zmin =1e5;
    LOOP(i,numD){
        if (D[i].x > xmax) { xmax = D[i].x; }
        if (D[i].x < xmin) { xmin = D[i].x; }
        if (D[i].y > ymax) { ymax = D[i].y; }
        if (D[i].y < ymin) { ymin = D[i].y; }
        if (D[i].z > zmax) { zmax = D[i].z; }
        if (D[i].z < zmin) { zmin = D[i].z; }
    }
    //Normalize
    double xrange = 0.5*(xmax - xmin), yrange = 0.5*(ymax - ymin), zrange = 0.5*(zmax - zmin);
    double dmax = xrange;
    if (dmax < yrange) {	dmax = yrange;	}
    if (dmax < zrange) {	dmax = zrange;	}
    LOOP(i,numD) {
        D[i].x = (D[i].x - xmin) / dmax + 0.5 * (ALPHA - 1.0) * xrange / dmax;
        D[i].y = (D[i].y - ymin) / dmax + 0.5 * (ALPHA - 1.0) * yrange / dmax;
        D[i].z = (D[i].z - zmin) / dmax + 0.5 * (ALPHA - 1.0) * zrange / dmax;
    }
    //Make supertetrahedron
    double x = ALPHA*xrange / dmax, y = ALPHA*yrange / dmax, z = ALPHA*zrange / dmax;
    MakeSupertetrahedron(_pelements, x, y, z);
    //Make rough mesh
    MakeRoughMesh(_pelements);
    //Delete needless elements
    DeleteSupertetrahedron(_pelements);
    //Make fine mesh
    //Renormalize cordinate
    LOOP(i,numD){
        D[i].x = D[i].x * dmax - 0.5*(ALPHA - 1.0)*xrange + xmin;
        D[i].y = D[i].y * dmax - 0.5*(ALPHA - 1.0)*yrange + ymin;
        D[i].z = D[i].z * dmax - 0.5*(ALPHA - 1.0)*zrange + zmin;
    }
}