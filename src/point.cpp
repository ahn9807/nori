//
//  point.cpp
//  Half
//
//  Created by junho on 2019. 7. 16..
//

#include <nori/emitter.h>
#include <nori/scene.h>
#include <nori/warp.h>

NORI_NAMESPACE_BEGIN

class PointLight : public Emitter {
public:
    PointLight(const PropertyList &props) {
        m_radiance = props.getColor("radiance", 10);
        m_position = props.getPoint("position", 0);
    }
    
    void setMesh(Mesh* mesh) {
        m_mesh = nullptr;
    }
    
    Color3f sample(EmitterQueryRecord &eqr, Sampler *sample) {
        eqr.pos = m_position;
        m_pdf = 1.f;
        return m_radiance;
    }
    
    float pdf(const EmitterQueryRecord &eqr) {
        return 1.f;
    }
    
    Color3f Le(const EmitterQueryRecord &eqr) const {
        return m_radiance;
    }
    
    bool rayIntersect(const Scene* scene, Ray3f &shadowRay, Intersection &its) const {
        if(!scene->rayIntersect(shadowRay, its)) {
            return false;
        }
        if(its.mesh->getEmitter() == this)
            return false;
        
        return true;
    }
    bool rayIntersect(const Scene* scene, Ray3f &shadowRay) const {
        Intersection its;
        return rayIntersect(scene, shadowRay, its);
    }
    
    bool isDeltaLight() const {
        return true;
    }
    
    std::string toString() const {
        return "PointLight[]";
    }
    
private:
    Color3f m_radiance;
    Point3f m_position;
    float m_pdf;
    Mesh* m_mesh;
};


NORI_REGISTER_CLASS(PointLight, "point");
NORI_NAMESPACE_END
