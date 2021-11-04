#include <algorithm>
#include <cassert>
#include "BVH.hpp"

BVHAccel::BVHAccel(std::vector<Object*> p, int maxPrimsInNode,
                   SplitMethod splitMethod)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(splitMethod),
      primitives(std::move(p))
{
    time_t start, stop;
    time(&start);
    if (primitives.empty())
        return;
    if(splitMethod==SplitMethod::NAIVE) {
        root = recursiveBuild(primitives);
    }
    if(splitMethod==SplitMethod::SAH){
        root=SAHrecursiveBuild(primitives);
    }
    time(&stop);
    double diff = difftime(stop, start);
    int hrs = (int)diff / 3600;
    int mins = ((int)diff / 60) - (hrs * 60);
    int secs = (int)diff - (hrs * 3600) - (mins * 60);

    printf(
        "\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
        hrs, mins, secs);
}
BVHBuildNode* BVHAccel::SAHrecursiveBuild(std::vector<Object*> objects){
    //区别即是
    BVHBuildNode* node=new BVHBuildNode();
    Bounds3 bounds;
    for(int i=0;i<objects.size();i++){
        bounds =Union(bounds,objects[i]->getBounds());
    }
    if(objects.size()==1){
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    else if(objects.size()==2){
        node->left = recursiveBuild(std::vector{objects[0]});
        node->right = recursiveBuild(std::vector{objects[1]});
        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else{
        //这里采取按物体的排序来取，用最长轴划分
        Bounds3 centroidBounds;
        for (int i = 0; i < objects.size(); ++i)
            centroidBounds =
                    Union(centroidBounds, objects[i]->getBounds().Centroid());
        int dim = centroidBounds.maxExtent();
        switch (dim) {
            case 0:
                std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                    return f1->getBounds().Centroid().x <
                           f2->getBounds().Centroid().x;
                });
                break;
            case 1:
                std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                    return f1->getBounds().Centroid().y <
                           f2->getBounds().Centroid().y;
                });
                break;
            case 2:
                std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                    return f1->getBounds().Centroid().z <
                           f2->getBounds().Centroid().z;
                });
                break;
        }

        std::vector<Bounds3> Buckets(12);//十二个桶,
        int groupsize[12];for(int i=0;i<12;++i)groupsize[i]=0;
        int bucket=0;
        switch (dim){
            case 0:
                for(int i=0;i<objects.size();++i){
                    bucket=12*centroidBounds.Offset(objects[i]->getBounds().Centroid()).x;
                    if(bucket==12)bucket=11;//边界
                    Buckets[bucket]=Union(Buckets[bucket],objects[i]->getBounds());
                    groupsize[bucket]++;
                }
                break;
            case 1:
                for(int i=0;i<objects.size();++i){
                    bucket=12*centroidBounds.Offset(objects[i]->getBounds().Centroid()).y;
                    if(bucket==12)bucket=11;//边界
                    Buckets[bucket]=Union(Buckets[bucket],objects[i]->getBounds());
                    groupsize[bucket]++;
                }
                break;
            case 2:
                for(int i=0;i<objects.size();++i){
                    bucket=12*centroidBounds.Offset(objects[i]->getBounds().Centroid()).z;
                    if(bucket==12)bucket=11;//边界
                    Buckets[bucket]=Union(Buckets[bucket],objects[i]->getBounds());
                    groupsize[bucket]++;
                }
                break;
        }
        //计算cost
        double cost[11];
        for(int i=0;i<11;++i){
            Bounds3 BL,BR;int NL=0,NR=0;
            for(int j=0;j<i+1;++j){
                BL=Union(BL,Buckets[j]);
                NL+=groupsize[j];
            }
            for(int j=i+1;j<12;++j){
                BR=Union(BR,Buckets[j]);
                NR+=groupsize[j];
            }
            cost[i]=(NL*BL.SurfaceArea()+NR*BR.SurfaceArea())/bounds.SurfaceArea();
        }
        //找到最小cost
        int FinalPartion=0;double minCost=cost[0];
        for(int i=0;i<11;++i){
            if(cost[i]<minCost){FinalPartion=i;minCost=cost[i];}
        }
        //计算划分的元素个数
        int ObjectNum=0;
        for(int i=0;i<=FinalPartion;++i){
            ObjectNum+=groupsize[i];
        }
        auto beginning = objects.begin();
        auto middling = objects.begin() +ObjectNum;
        auto ending = objects.end();
        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);

        assert(objects.size() == (leftshapes.size() + rightshapes.size()));

        node->left = SAHrecursiveBuild(leftshapes);
        node->right = SAHrecursiveBuild(rightshapes);

        node->bounds = Union(node->left->bounds, node->right->bounds);
    }
    return node;
}

BVHBuildNode* BVHAccel::recursiveBuild(std::vector<Object*> objects)
{
    BVHBuildNode* node = new BVHBuildNode();

    // Compute bounds of all primitives in BVH node
    Bounds3 bounds;
    for (int i = 0; i < objects.size(); ++i)
        bounds = Union(bounds, objects[i]->getBounds());
    if (objects.size() == 1) {
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    else if (objects.size() == 2) {
        node->left = recursiveBuild(std::vector{objects[0]});
        node->right = recursiveBuild(std::vector{objects[1]});

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else {
        Bounds3 centroidBounds;
        for (int i = 0; i < objects.size(); ++i)
            centroidBounds =
                Union(centroidBounds, objects[i]->getBounds().Centroid());
        int dim = centroidBounds.maxExtent();
        switch (dim) {
        case 0:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x <
                       f2->getBounds().Centroid().x;
            });
            break;
        case 1:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y <
                       f2->getBounds().Centroid().y;
            });
            break;
        case 2:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z <
                       f2->getBounds().Centroid().z;
            });
            break;
        }

        auto beginning = objects.begin();
        auto middling = objects.begin() + (objects.size() / 2);
        auto ending = objects.end();

        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);

        assert(objects.size() == (leftshapes.size() + rightshapes.size()));

        node->left = recursiveBuild(leftshapes);
        node->right = recursiveBuild(rightshapes);

        node->bounds = Union(node->left->bounds, node->right->bounds);
    }

    return node;
}

Intersection BVHAccel::Intersect(const Ray& ray) const
{
    Intersection isect;
    if (!root)
        return isect;
    isect = BVHAccel::getIntersection(root, ray);
    return isect;
}

Intersection BVHAccel::getIntersection(BVHBuildNode* node, const Ray& ray) const
{
    // TODO Traverse the BVH to find intersection
    Bounds3 Bound = node->bounds;
    std::array<int, 3> dirIsNeg;
    if (ray.direction.x > 0) { dirIsNeg[0] = 0; } else { dirIsNeg[0] = 1; }
    if (ray.direction.y > 0) { dirIsNeg[1] = 0; } else { dirIsNeg[1] = 1; }
    if (ray.direction.z > 0) { dirIsNeg[2] = 0; } else { dirIsNeg[2] = 1; }
    Vector3f invDir = ray.direction_inv;
    if (Bound.IntersectP(ray, invDir, dirIsNeg)) {
        if(node->left== nullptr&&node->right== nullptr){
            //叶子结点
            return node->object->getIntersection(ray);
        }
        else{
            //内部的节点
            Intersection hitl=getIntersection(node->left,ray);
            Intersection hitr=getIntersection(node->right,ray);
            return (hitl.distance<hitr.distance) ? hitl:hitr;
        }
    }
    return Intersection();//不相交
}
