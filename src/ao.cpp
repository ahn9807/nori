//
//  ao.cpp
//  Half
//
//  Created by Junho on 19/06/2019.
//

#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/warp.h>

#define ALPHA = MAXFLOAT;

NORI_NAMESPACE_BEGIN

class AmbientOcclusionIntegrator : public Integrator {
public:
    AmbientOcclusionIntegrator(const PropertyList &props) {

    }
    
    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
        /* Find the surface that is visible in the requested direction */
        Intersection its;
        if(!scene->rayIntersect(ray, its)) {
            return Color3f(0.f);
        }
        
        Color3f returnColor = Color3f(0.f);
        Point3f sample;
        Point3f samplePoint;
        Ray3f shadowRay;
        float cosine;
        sample = Warp::squareToUniformHemisphere(sampler->next2D());
        samplePoint = its.toWorld(sample);
        shadowRay = Ray3f(its.p,samplePoint);
        
        if(!scene->rayIntersect(shadowRay)) {
            its.shFrame.n.normalize();
            samplePoint.normalize();
            cosine = samplePoint.dot(its.shFrame.n);
            returnColor = Color3f((cosine / M_PI)) / Warp::squareToUniformHemispherePdf(sample);
        }
        
        return returnColor;
    }
    
    std::string toString() const {
        return "AmbientOcculusionIntegrator[]";
    }
    
private:

};

NORI_REGISTER_CLASS(AmbientOcclusionIntegrator, "ao");
NORI_NAMESPACE_END
