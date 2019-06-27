#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/emitter.h>
#include <nori/bsdf.h>
#include <nori/dpdf.h>

NORI_NAMESPACE_BEGIN

class PathBruteIntegrator : public Integrator {
public:
    PathBruteIntegrator(const PropertyList &props) {
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
        
        BSDFQueryRecord bsdfQ = BSDFQueryRecord(its.toLocal(-ray.d));
        Color3f albedo = its.mesh->getBSDF()->sample(bsdfQ, Point2f(drand48(), drand48()));
        Color3f light = Color3f(0.f);
        if(its.mesh->isEmitter()) {
            light = its.mesh->getEmitter()->Le(its.p, its.shFrame.n, its.toWorld(bsdfQ.wo));
        }
        
        //russian Roulette
        if(drand48() < 0.99f)
            return 1.f/0.99 * (light + albedo * Li(scene, sampler, Ray3f(its.p, its.toWorld((bsdfQ.wo)))));
        else
            return Color3f(0.f);
        
    }
    
    std::string toString() const {
        return "WhittedIntegrator[]";
    }
private:
    std::vector<Mesh*> lights;
    DiscretePDF m_light_pdf;
};

NORI_REGISTER_CLASS(PathBruteIntegrator, "path_mats");
NORI_NAMESPACE_END
