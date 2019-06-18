//
//  octree.h
//  nori
//
//  Created by Junho on 13/06/2019.
//


#pragma once

#include <nori/bbox.h>
#include <vector>

NORI_NAMESPACE_BEGIN

//node for octree
class OctNode {
public:
    //생성자와 소멸자
    OctNode(Point3f a, Point3f b, int depth) {
        Point3f min = Point3f(std::min(a.x(),b.x()),
                              std::min(a.y(),b.y()),
                              std::min(a.z(),b.z()));
        Point3f max = Point3f(std::max(a.x(),b.x()),
                              std::max(a.y(),b.y()),
                              std::max(a.z(),b.z()));
                              this->bbox = new BoundingBox3f(min,max);
        this->indices = new std::vector<int>();
        this->depth = depth;
        this->children = nullptr;
    }
    ~OctNode() {
        delete children;
        delete indices;
        delete bbox;
    }
    //자식 노드 생성
    void buildChildren(const Mesh *mesh);
    //검색
    void search(const Ray3f &ray, const Mesh* mesh, int &index, float &distance);
    //변수 접근자
    BoundingBox3f* getBoundBoxPtr() {   return bbox;    }
    std::vector<OctNode*>* getChildern() {    return children;    }
    std::vector<int>* getIndicesPtr() {     return indices;     }
    //정렬
    void sortChildrenByRay(const Ray3f &ray);
    
private:
    std::vector<OctNode*>* children;
    BoundingBox3f* bbox;
    std::vector<int>* indices;
    int depth;
};

class Octree {
public:
    void buildOctree(const Mesh *m);
    void search(const Ray3f &ray, int &index) const;
private:
    OctNode* root;
    const Mesh *mesh;
};

NORI_NAMESPACE_END

/*
#pragma once

#include <nori/bbox.h>
#include <vector>

NORI_NAMESPACE_BEGIN

//node for octree
class OctNode {
public:
    //생성자와 소멸자
    OctNode(Point3f a, Point3f b, int depth) {
        Point3f min = Point3f(std::min(a.x(),b.x()),
                              std::min(a.y(),b.y()),
                              std::min(a.z(),b.z()));
        Point3f max = Point3f(std::max(a.x(),b.x()),
                              std::max(a.y(),b.y()),
                              std::max(a.z(),b.z()));
        this->bbox = new BoundingBox3f(min,max);
        this->indices = new std::vector<int>();
        this->depth = depth;
        for(int i=0;i<8;i++) {
            children[i] = nullptr;
        }
    }
    ~OctNode() {
        delete indices;
        delete bbox;
        for(int i=0;i<8;i++) {
            delete children[i];
        }
        delete *children;
    }
    //자식 노드 생성
    void buildChildren(const Mesh *mesh);
    //검색
    void search(const Ray3f &ray, const Mesh* mesh, int &index, float &distance);
    //변수 접근자
    BoundingBox3f* getBoundBoxPtr() {   return bbox;    }
    OctNode** getChildern() {    return children;    }
    std::vector<int>* getIndicesPtr() {     return indices;     }
    //정렬
    void sortChildrenByRay(const Ray3f &ray);
    
private:
    OctNode* children[8];
    BoundingBox3f* bbox;
    std::vector<int>* indices;
    int depth;
};

class Octree {
public:
    void buildOctree(const Mesh *m);
    void search(const Ray3f &ray, int &index) const;
private:
    OctNode* root;
    const Mesh *mesh;
};

NORI_NAMESPACE_END
*/

