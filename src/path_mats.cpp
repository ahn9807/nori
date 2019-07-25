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

    }
    
    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray, int depth = 0) const {
        Intersection its;
        if (!scene->rayIntersect(ray, its)) {
            if (scene->getEnvmentLight() != nullptr) {
                EmitterQueryRecord eqr = EmitterQueryRecord(ray.o, ray.d);
                return scene->getEnvmentLight()->Le(eqr);
            }
            
            return 0.f;
        }
        
        BSDFQueryRecord bsdfQ = BSDFQueryRecord(its.toLocal(-ray.d));
        bsdfQ.uv = its.uv;
        Color3f albedo = its.mesh->getBSDF()->sample(bsdfQ, Point2f(drand48(), drand48()));
        
        Color3f light = Color3f(0.f);
        if(its.mesh->isEmitter()) {
            EmitterQueryRecord erq = EmitterQueryRecord(its.p, its.shFrame.n, -its.toWorld(bsdfQ.wo));
            light = its.mesh->getEmitter()->Le(erq);
        }
        
        //russian Roulette
        if(drand48() < 0.99f)
            return 1.f/0.99 * (light + albedo * Li(scene, sampler, Ray3f(its.p, its.toWorld((bsdfQ.wo)))));
        else
            return Color3f(0.f);
        
    }
    
    std::string toString() const {
        return "MatIntegrator[]";
    }
private:

};

NORI_REGISTER_CLASS(PathBruteIntegrator, "path_mats");
NORI_NAMESPACE_END
