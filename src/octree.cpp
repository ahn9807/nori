//
//  octree.cpp
//  Half
//
//  Created by Junho on 15/06/2019.
//


#include <nori/octree.h>
#include <nori/mesh.h>
#include <algorithm>

#define MAXDEPTH 15
#define CHILDNUM 10

NORI_NAMESPACE_BEGIN

void OctNode::buildChildren(const Mesh *mesh) {
    //깊이 체크
    if(depth > MAXDEPTH)
        return;
    
    Point3f center = bbox->getCenter();

    children = new std::vector<OctNode*>();
    
    for(int i=0;i<8;i++) {
        children->push_back(new OctNode(center, bbox->getCorner(i), depth+1));
    }
    
    for(auto child:*children) {
        for(std::vector<int>::size_type j=0;j<indices->size();j++) {
            if(child->bbox->overlaps(mesh->getBoundingBox(indices->at(j)))) {
                child->indices->push_back(indices->at(j));
            }
        }
    }
    
    for(std::vector<OctNode*>::size_type i = 0; i < 8; i++) {
        if(children->back()->indices->size() == 0) {
            delete children->back();
            children->pop_back();
        }
    }
    
    delete indices;
    indices = nullptr;
    
    for(auto child:*children) {
        if(child->indices->size() > CHILDNUM) {
            child->buildChildren(mesh);
        }
    }
}

void OctNode::search(const Ray3f &ray, const Mesh* mesh, int &index, float &distance) {
    //When node is Leaf node;
    if(children == nullptr) {
        for(std::vector<int>::size_type j=0;j<indices->size();j++) {
            float u,v;
            float t = distance;
            mesh->rayIntersect(indices->at(j), ray, u, v, t);
            if(t < distance) {
                index = indices->at(j);
                distance = t;
            }
        }
        return;
    }
    
    for(auto child:*children) {
        if(child->bbox->rayIntersect(ray)) {
            child->search(ray, mesh, index, distance);
        }
    }
}

void Octree::buildOctree(const Mesh *mesh) {
    this->mesh = mesh;
    root = new OctNode(mesh->getBoundingBox().min,mesh->getBoundingBox().max, 0);
    for(uint i=0;i<mesh->getTriangleCount();i++) {
        root->getIndicesPtr()->push_back(i);
    }
    root->buildChildren(mesh);
}

void Octree::search(const Ray3f &ray, int &index) const {
    float distance = MAXFLOAT;
    index = -1;

    root->search(ray, mesh, index, distance);
}



NORI_NAMESPACE_END


/*
#include <nori/octree.h>
#include <nori/mesh.h>
#include <algorithm>

#define MAXDEPTH 30
#define CHILDNUM 10

NORI_NAMESPACE_BEGIN

void OctNode::buildChildren(const Mesh *mesh) {
    //깊이 체크
    if(depth > MAXDEPTH)
        return;
    
    Point3f center = bbox->getCenter();
    
    for(int i=0;i<8;i++) {
        children[i] = new OctNode(center, bbox->getCorner(i), depth+1);
    }
    
    for(int i=0;i<8;i++) {
        for(std::vector<int>::size_type j=0;j<indices->size();j++) {
            if(children[i]->bbox->overlaps(mesh->getBoundingBox(indices->at(j)))) {
                children[i]->indices->push_back(indices->at(j));
            }
        }
    }
    
    for(int i=0;i<8;i++) {
        if(children[i]->indices->size() == 0)
        {
            delete children[i];
            children[i] = nullptr;
        }
    }
    
    delete indices;
    indices = nullptr;
    
    for(int i=0;i<8;i++) {
        if(children[i] != nullptr && children[i]->indices->size() > CHILDNUM) {
            children[i]->buildChildren(mesh);
        }
    }
}

void OctNode::search(const Ray3f &ray, const Mesh* mesh, int &index, float &distance) {
    //When node is Leaf node;
    if(indices != nullptr) {
        for(std::vector<int>::size_type j=0;j<indices->size();j++) {
            float u,v;
            float t = distance;
            mesh->rayIntersect(indices->at(j), ray, u, v, t);
            if(t < distance) {
                index = indices->at(j);
                distance = t;
            }
        }
        return;
    }
    
    for(int i=0;i<8;i++) {
        if(children[i] != nullptr && children[i]->bbox->rayIntersect(ray)) {
            children[i]->search(ray, mesh, index, distance);
        }
    }
}

void Octree::buildOctree(const Mesh *mesh) {
    this->mesh = mesh;
    root = new OctNode(mesh->getBoundingBox().min,mesh->getBoundingBox().max, 0);
    for(uint i=0;i<mesh->getTriangleCount();i++) {
        root->getIndicesPtr()->push_back(i);
    }
    root->buildChildren(mesh);
}

void Octree::search(const Ray3f &ray, int &index) const {
    float distance = MAXFLOAT;
    index = -1;
    
    root->search(ray, mesh, index, distance);
}



NORI_NAMESPACE_END
*/
