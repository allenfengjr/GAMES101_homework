//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"
#include <omp.h>

void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    //随机更新?为什么==>因为ray要打到不同的光源上,类似蒙特卡洛?
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);//在这里更新了pos和pdf
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TO DO Implement Path Tracing Algorithm here
    Intersection inter=intersect(ray);
    Material* m=inter.m;//交点材质
    if(!inter.happened)
        return Vector3f(0);
    if(m->hasEmission())
        return m->getEmission();
    Vector3f Li_dir;
    Vector3f Li_indir;
    Vector3f Li=inter.emit;
    Vector3f n=normalize(inter.normal);//交点平面的法线
    Vector3f pos=inter.coords;
    Vector3f wo=normalize(-ray.direction);//wo 原来是from eye to p,现在是 from p to eye
    float pdf;
    Intersection light_inter;
    //Li_dir部分
    sampleLight(light_inter,pdf);//更新light_inter,pdf
    Vector3f nn=normalize(light_inter.normal);//即n',但这里的nn,n方向朝哪里不太确定,要试试正负
    Vector3f x=light_inter.coords;//sample光源的点
    Vector3f ws=normalize(x-pos);//,ws按from p to x做
    Intersection getLight=intersect(Ray(inter.coords,ws));//从交点往光源打一条光，看看是否被阻挡
    //std::cout<<"light distance: "<<getLight.distance<<std::endl;
    //std::cout<<"(x-pos.norm()): "<<(x-pos).norm()<<std::endl;
    if(abs(getLight.distance-(x-pos).norm())<0.001){
        //肯定会打中目标,判断是不是光源即可
        Li_dir =light_inter.emit * m->eval(wo, ws, n) * dotProduct(ws, n) * dotProduct(-ws,nn) / ((x-pos).norm()*(x-pos).norm()*pdf) ;
    }
    if(get_random_float()<RussianRoulette){
        //间接光照
        Vector3f wi=normalize(m->sample(wo,n));//如何反射是和材质有关的
        Ray q(pos,wi);
        Intersection qi=intersect(q);
        if(qi.m!=nullptr&&!qi.m->hasEmission()) {
            Li_indir = castRay(q, depth) * m->eval(wo, wi, n) * dotProduct(wi, n) / m->pdf(wo, wi, n) / RussianRoulette;
        }
        //一个疑问，这里到底要不要判断是否打得是光源?==>当然要,但核心不是判断打中的是发光体还是不发光的
        //核心是如果打得是发光体，直接略过，否则能量不守恒
    }
    return Li_dir+Li_indir;
}