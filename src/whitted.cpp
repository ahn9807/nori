//
//  whitted.cpp
//  Half
//
//  Created by Junho on 19/06/2019.
//

#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/emitter.h>
#include <nori/bsdf.h>
#include <nori/dpdf.h>

NORI_NAMESPACE_BEGIN

class WhittedIntegrator : public Integrator {
public:
    WhittedIntegrator(const PropertyList &props) {
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

        //preprocessing for caculation
        Mesh* randomLight = lights.at(m_light_pdf.sample(drand48()));
        Point3f lightSamplePosition;
        Normal3f lightSampleNormal;
        randomLight->sample(sampler, lightSamplePosition, lightSampleNormal);
        Emitter* emit = randomLight->getEmitter();
        Vector3f distanceVec = lightSamplePosition - its.p;
        Color3f directEmit = Color3f(0);

        //for calculatin G(x<->y)
        float distance = distanceVec.dot(distanceVec);
        distanceVec.normalize();
        float objectNormal = abs(its.shFrame.n.dot(distanceVec));
        float lightNormal = abs(lightSampleNormal.dot(-distanceVec));
        Ray3f shadowRay = Ray3f(its.p, distanceVec);
        
        //check current its.p is emitter() then distnace -> infinite
        if(its.mesh->isEmitter())
            directEmit = its.mesh->getEmitter()->Le(lightSamplePosition, lightSampleNormal, -ray.d);
        
        //BRDF of given its.p material
        BSDFQueryRecord bsdfQ = BSDFQueryRecord(its.toLocal(-ray.d), its.toLocal(distanceVec), ESolidAngle);
        Color3f bsdf = its.mesh->getBSDF()->eval(bsdfQ);
        
        //Pdf value for light (diffuse area light)
        float lightPdf = 1.f/m_light_pdf.getSum();
        
        //MC integral
        if(!emit->rayIntersect(scene, shadowRay)) {
            return directEmit + Color3f(1.f/distance) * objectNormal * lightNormal * emit->Le(lightSamplePosition, lightSampleNormal, -shadowRay.d) * bsdf * 1.f/lightPdf;
        }
        else {
            return Color3f(0.f);
        }
    }
    
    std::string toString() const {
        return "WhittedIntegrator[]";
    }
private:
    std::vector<Mesh*> lights;
    DiscretePDF m_light_pdf;
};

NORI_REGISTER_CLASS(WhittedIntegrator, "whitted");
NORI_NAMESPACE_END
