//
//  path_vol.cpp
//  Half
//
//  Created by Junho on 25/07/2019.
//


#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/emitter.h>
#include <nori/bsdf.h>
#include <nori/dpdf.h>
#include <nori/warp.h>

NORI_NAMESPACE_BEGIN

class PathVolIntegrator : public Integrator {
public:
    PathVolIntegrator(const PropertyList &props) {
        /* No parameters this time */
    }
    
    void preprocess(const Scene *scene) {
        
    }
    
    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray, int depth = 0) const {
        return 0.f;
    }
    
    std::string toString() const {
        return "VolumePathIntegrator[]";
    }
private:

};

NORI_REGISTER_CLASS(PathVolIntegrator, "path_vol");
NORI_NAMESPACE_END
