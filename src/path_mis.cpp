//
//  path_mis.cpp
//  Half
//
//  Created by Junho on 25/06/2019.
//

#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/emitter.h>
#include <nori/bsdf.h>
#include <nori/dpdf.h>

NORI_NAMESPACE_BEGIN

class PathMisIntegrator : public Integrator {
public:
    PathMisIntegrator(const PropertyList &props) {
        /* No parameters this time */
    }
    
    void preprocess(const Scene *scene) {
        for(auto object:scene->getMeshes()) {
            if(object->isEmitter())
                lights.push_back(object);
        }
        
        m_light_pdf = DiscretePDF(lights.size());
        for(auto object:lights) {
            m_light_pdf.append(object->getTotalSurfaceArea());
        }
        m_light_pdf.normalize();
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
        
        return SamplingLight(scene, sampler, ray) + SamplingBSDF(scene, sampler, ray);
    }
    
    Color3f SamplingLight(const Scene *scene, Sampler *sampler, const Ray3f &ray, int depth = 0) const {
        Intersection its;
        if (!scene->rayIntersect(ray, its))
            return Color3f(0.0f);
        
        //preprocessing for caculation
        Mesh* randomLight = lights.at((int)(drand48() * lights.size()));
        Point3f lightSamplePosition;
        Normal3f lightSampleNormal;
        randomLight->sample(sampler, lightSamplePosition, lightSampleNormal);
        Emitter* emit = randomLight->getEmitter();
        Vector3f distanceVec = lightSamplePosition - its.p;
        Color3f directColor = Color3f(0.f);
        
        //for calculatin G(x<->y)
        float distance = distanceVec.dot(distanceVec);
        distanceVec.normalize();
        float objectNormal = abs(its.shFrame.n.dot(distanceVec));
        float lightNormal = abs(lightSampleNormal.dot(-distanceVec));
        Ray3f shadowRay = Ray3f(its.p, distanceVec);
        
        //check current its.p is emitter() then distnace -> infinite
        if(its.mesh->isEmitter()) {
            if(depth == 0)
                return its.mesh->getEmitter()->Le(its.p, its.shFrame.n, -ray.d);
            else
                return Color3f(0.f);
        }
        
        //BRDF of given its.p material
        BSDFQueryRecord bsdfQ = BSDFQueryRecord(its.toLocal(-ray.d), its.toLocal(distanceVec), ESolidAngle);
        Color3f bsdf = its.mesh->getBSDF()->eval(bsdfQ);
        
        //Pdf value for light (diffuse area light)
        float lightPdf = 1.f/lights.size() * 1.f/randomLight->getTotalSurfaceArea();
        
        //Weighting
        float modifiedLightPdf = lightPdf * distance;
        float bsdfPdf = its.mesh->getBSDF()->pdf(bsdfQ);
        float weight = modifiedLightPdf/(modifiedLightPdf+ bsdfPdf);
        Color3f albedo = its.mesh->getBSDF()->sample(bsdfQ, Point2f(drand48(), drand48()));
        
        //MC integral
        if(!emit->rayIntersect(scene, shadowRay)) {
            directColor = Color3f(1.f/distance) * objectNormal * lightNormal * emit->Le(lightSamplePosition, lightSampleNormal, -shadowRay.d) * bsdf * 1.f/lightPdf;
        }
        
        return weight * 1.f/0.99 * (directColor + albedo * SamplingLight(scene, sampler, Ray3f(its.p, its.toWorld((bsdfQ.wo))), depth + 1));
    }
    
    Color3f SamplingBSDF(const Scene *scene, Sampler *sampler, const Ray3f &ray, int depth = 0) const {
        Intersection its;
        if (!scene->rayIntersect(ray, its)) {
            return Color3f(0.0f);
        }

        BSDFQueryRecord bsdfQ = BSDFQueryRecord(its.toLocal(-ray.d));
        Color3f albedo = its.mesh->getBSDF()->sample(bsdfQ, Point2f(drand48(), drand48()));
        Color3f light = Color3f(0.f);
        if(its.mesh->isEmitter()) {
            light = its.mesh->getEmitter()->Le(its.p, its.shFrame.n, its.toWorld(bsdfQ.wo));
        }
        
        //Weighting
        float lightPdf = 0.f;
        Intersection lightInsect;
        if(scene->rayIntersect(Ray3f(its.p, its.toWorld((bsdfQ.wo))), lightInsect)) {
            if(lightInsect.mesh->isEmitter()) {
                lightPdf = 1.f/lights.size() * 1.f/lightInsect.mesh->getTotalSurfaceArea();
                Vector3f distanceVec = lightInsect.p - its.p;
                float distance = distanceVec.dot(distanceVec);
                lightPdf *= distance;
            }
        }
        float bsdfPdf = its.mesh->getBSDF()->pdf(bsdfQ);
        float weight = bsdfPdf/(bsdfPdf + lightPdf);
        if(bsdfPdf == 0 && lightPdf == 0) {
            weight = 1;
        }
        
        //russian Roulette
        if(drand48() < 0.99f)
            return weight * 1.f/0.99 * (light + albedo * SamplingBSDF(scene, sampler, Ray3f(its.p, its.toWorld((bsdfQ.wo))), depth + 1));
        else
            return Color3f(0.f);
    }
    

    
    std::string toString() const {
        return "PathEmsIntegrator[]";
    }
private:
    std::vector<Mesh*> lights;
    DiscretePDF m_light_pdf;
};

NORI_REGISTER_CLASS(PathMisIntegrator, "path_mis");
NORI_NAMESPACE_END

