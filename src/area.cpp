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
        m_radiance = props.getColor("radiance");
    }
    
    Color3f sample(Sampler *sampler) const {
            return m_radiance;
    }
    
    Color3f Le(const Point3f &p, const Normal3f &n, const Vector3f &w_o) const {
        // return n.dot(w_o) > 0.f ? m_radiance : 0.f;
        return m_radiance;
    }
    
    bool rayIntersect(const Scene* scene, Ray3f &shadowRay, Intersection &its) const {
        if(!scene->rayIntersect(shadowRay, its)) {
            return false;
        }
        if(its.mesh->isEmitter())
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
};


NORI_REGISTER_CLASS(AreaLight, "area");
NORI_NAMESPACE_END
