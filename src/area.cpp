//
//  area.cpp
//  Half
//
//  Created by Junho on 19/06/2019.
//

#include <nori/emitter.h>
#include <nori/scene.h>
#include <nori/warp.h>

NORI_NAMESPACE_BEGIN

class AreaLight : public Emitter {
public:
    AreaLight(const PropertyList &props) {
        m_radiance = props.getColor("radiance",32);
    }
    
    void setMesh(Mesh* mesh) {
        m_mesh = mesh;
    }
    
    Color3f sample(EmitterQueryRecord &eqr, Sampler *sample) {
        m_mesh->sample(sample, eqr.pos, eqr.normal);
        eqr.wi = eqr.pos - eqr.ref;
        m_pdf = m_mesh->getTotalSurfaceArea();
        return Le(eqr) * m_pdf;
    }
    
    float pdf(const EmitterQueryRecord &eqr) {
        m_pdf = m_mesh->getTotalSurfaceArea();
        return 1.f/m_pdf;
    }
    
    Color3f Le(const EmitterQueryRecord &eqr) const {
        return  -eqr.wi.dot(eqr.normal) > 0.f ? m_radiance : 0.f;
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
        return false;
    }
    
    std::string toString() const {
        return "AreaLight[]";
    }
    
private:
    Color3f m_radiance;
    float m_pdf;
    Mesh* m_mesh;
};


NORI_REGISTER_CLASS(AreaLight, "area");
NORI_NAMESPACE_END
