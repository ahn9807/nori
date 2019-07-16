//
//  path_ems.cpp
//  Half
//
//  Created by Junho on 24/06/2019.
//

#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/emitter.h>
#include <nori/bsdf.h>
#include <nori/dpdf.h>

NORI_NAMESPACE_BEGIN

class PathEmsIntegrator : public Integrator {
public:
    PathEmsIntegrator(const PropertyList &props) {
        /* No parameters this time */
    }
    
    void preprocess(const Scene *scene) {

    }
    
    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray, int depth = 0) const {
        Intersection its;
        if (!scene->rayIntersect(ray, its))
            return Color3f(0.0f);
        
        if(!its.mesh->getBSDF()->isDiffuse()) {
            if(drand48() > 0.95)
                return Color3f(0.f);
            else {
                BSDFQueryRecord bsdfQ = BSDFQueryRecord(its.toLocal(-ray.d));
                its.mesh->getBSDF()->sample(bsdfQ, Point2f(drand48(), drand48()));
                return 1.057 * Li(scene, sampler, Ray3f(its.p, its.toWorld(bsdfQ.wo)));
            }
        }
        
        //preprocessing for caculation
        Emitter* emit = scene->getRandomEmitter(sampler);
        EmitterQueryRecord eqr = EmitterQueryRecord(its.p);
        Color3f Le = emit->sample(eqr, sampler);
        Vector3f distanceVec = eqr.pos - its.p;
        Color3f directColor = Color3f(0.f);
        
        //for calculatin G(x<->y)
        float distance = distanceVec.dot(distanceVec);
        distanceVec.normalize();
        float objectNormal = abs(its.shFrame.n.dot(distanceVec));
        float lightNormal = abs(eqr.normal.dot(-distanceVec));
        if(emit->isDeltaLight())
            lightNormal = 1;
        Ray3f shadowRay = Ray3f(its.p, distanceVec);
        
        //check current its.p is emitter() then distnace -> infinite
        if(its.mesh->isEmitter()) {
            if(depth == 0)
                return Le;
            else
                return Color3f(0.f);
        }
        
        //BRDF of given its.p material
        BSDFQueryRecord bsdfQ = BSDFQueryRecord(its.toLocal(-ray.d), its.toLocal(distanceVec), ESolidAngle);
        Color3f bsdf = its.mesh->getBSDF()->eval(bsdfQ);
        Color3f albedo = its.mesh->getBSDF()->sample(bsdfQ, Point2f(drand48(), drand48()));
        
        //Pdf value for light (diffuse area light)
        float lightPdf = scene->getEmitterPdf();
        
        //MC integral
        if(!emit->rayIntersect(scene, shadowRay)) {
            directColor = Color3f(1.f/distance) * objectNormal * lightNormal * Le * bsdf * 1.f/lightPdf;
        }
        else {
            directColor = Color3f(0.f);
        }
        
        if(drand48() < 0.95)
            return 1.f/0.95 * (directColor + albedo * Li(scene, sampler, Ray3f(its.p, its.toWorld((bsdfQ.wo))),depth + 1));
        else
            return Color3f(0.f);
    }
    
    std::string toString() const {
        return "PathEmsIntegrator[]";
    }
private:
};

NORI_REGISTER_CLASS(PathEmsIntegrator, "path_ems");
NORI_NAMESPACE_END

