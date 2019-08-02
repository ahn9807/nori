#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/emitter.h>
#include <nori/bsdf.h>
#include <nori/dpdf.h>

NORI_NAMESPACE_BEGIN

class PathSpectralIntegrator : public Integrator {
public:
    PathSpectralIntegrator(const PropertyList &props) {
        /* No parameters this time */
    }
    
    void preprocess(const Scene *scene) {
        
    }
    
    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray, int depth = 0) const {
        float random = drand48();
        float sampleWaveLength = 380 + (750-380)*drand48();
        if(random < 0.33) {
            sampleWaveLength = 435.8f;
        }
        else if(random < 0.66) {
            sampleWaveLength = 546.1f;
        }
        else
            sampleWaveLength = 700.0f;
        
        sampleWaveLength = 380 + (750 - 380) * drand48();

        return Li_spectral_mat(scene, sampler, ray, sampleWaveLength);
    }
    
    Color3f Li_spectral_mat(const Scene *scene, Sampler *sampler, const Ray3f &ray, float waveLength) const {
        Intersection its;
        if (!scene->rayIntersect(ray, its)) {
            if (scene->getEnvmentLight() != nullptr) {
                EmitterQueryRecord eqr = EmitterQueryRecord(ray.o, ray.d);
                return scene->getEnvmentLight()->Le(eqr) * waveLengthToRGB(waveLength);
            }
            
            return 0.f;
        }
        
        BSDFQueryRecord bsdfQ = BSDFQueryRecord(its.toLocal(-ray.d));
        
        bsdfQ.uv = its.uv;
        
        Color3f light = Color3f(0.f);
        bsdfQ.waveLength = waveLength;
        Color3f albedo = its.mesh->getBSDF()->sample(bsdfQ, Point2f(drand48(), drand48()));
        
        if(its.mesh->isEmitter()) {
            EmitterQueryRecord erq = EmitterQueryRecord(its.p, its.shFrame.n, -its.toWorld(bsdfQ.wo));
            light = its.mesh->getEmitter()->Le(erq) * waveLengthToRGB(waveLength);
        }
        
        
        //russian Roulette
        if(drand48() < 0.99f)
            return 1.f/0.99 * (light + albedo * Li_spectral_mat(scene, sampler, Ray3f(its.p, its.toWorld((bsdfQ.wo))),waveLength));
        else
            return Color3f(0.f);
    }
    
    Color3f Li_spectral_ems(const Scene *scene, Sampler *sampler, const Ray3f &ray, int depth = 0, float waveLength = 566) const {
        Intersection its;
        if (!scene->rayIntersect(ray, its)) {
            if (scene->getEnvmentLight() != nullptr) {
                EmitterQueryRecord eqr = EmitterQueryRecord(ray.o, ray.d);
                return scene->getEnvmentLight()->Le(eqr) * waveLengthToRGB(waveLength);
            }
            
            return Color3f(0.f);
        }
        
        if(!its.mesh->getBSDF()->isDiffuse()) {
            if(drand48() > 0.95)
                return Color3f(0.f);
            else {
                BSDFQueryRecord bsdfQ = BSDFQueryRecord(its.toLocal(-ray.d));
                bsdfQ.uv = its.uv;
                bsdfQ.waveLength = waveLength;
                Color3f albedo = its.mesh->getBSDF()->sample(bsdfQ, Point2f(drand48(), drand48()));
                return 1.057 * albedo * Li(scene, sampler, Ray3f(its.p, its.toWorld(bsdfQ.wo)));
            }
        }
        
        //preprocessing for caculation
        Emitter* emit = scene->getRandomEmitter(sampler);
        EmitterQueryRecord eqr = EmitterQueryRecord(its.p);
        Color3f Le = emit->sample(eqr, sampler) * waveLengthToRGB(waveLength);
        Vector3f distanceVec = eqr.wi.normalized();
        Color3f directColor = Color3f(0.f);
        
        //for calculatin G(x<->y)
        float objectNormal = abs(its.shFrame.n.dot(distanceVec));
        
        //check current its.p is emitter() then distnace -> infinite
        if(its.mesh->isEmitter()) {
            if(depth == 0)
                return its.mesh->getEmitter()->Le(eqr) * waveLengthToRGB(waveLength);
            else
                return Color3f(0.f);
        }
        
        //BRDF of given its.p material
        BSDFQueryRecord bsdfQ = BSDFQueryRecord(its.toLocal(-ray.d), its.toLocal(distanceVec), ESolidAngle);
        bsdfQ.uv = its.uv;
        bsdfQ.waveLength = waveLength;
        Color3f bsdf = its.mesh->getBSDF()->eval(bsdfQ);
        Color3f albedo = its.mesh->getBSDF()->sample(bsdfQ, Point2f(drand48(), drand48()));
        
        //Pdf value for light (diffuse area light)
        float lightPdf = scene->getEmitterPdf();
        
        //MC integral
        if(!emit->rayIntersect(scene, eqr.shadowRay)) {
            directColor = objectNormal * Le * bsdf * 1.f/lightPdf;
        }
        else {
            directColor = Color3f(0.f);
        }
        
        if(drand48() < 0.95)
            return 1.f/0.95 * (directColor + albedo * Li(scene, sampler, Ray3f(its.p, its.toWorld((bsdfQ.wo))),depth + 1));
        else
            return Color3f(0.f);
    }
    
    float SpectralDensityFunction(float waveLength) {
        return 0.f;
    }
    
    std::string toString() const {
        return "PathSpectralIntegrator[]";
    }
    
    static Color3f waveLengthToRGB(double Wavelength){
        double factor;
        double Red,Green,Blue;
        double Gamma = 0.8;
        double IntensityMax = 1;
        
        if((Wavelength >= 380) && (Wavelength<440)){
            Red = -(Wavelength - 440) / (440 - 380);
            Green = 0.0;
            Blue = 1.0;
        }else if((Wavelength >= 440) && (Wavelength<490)){
            Red = 0.0;
            Green = (Wavelength - 440) / (490 - 440);
            Blue = 1.0;
        }else if((Wavelength >= 490) && (Wavelength<510)){
            Red = 0.0;
            Green = 1.0;
            Blue = -(Wavelength - 510) / (510 - 490);
        }else if((Wavelength >= 510) && (Wavelength<580)){
            Red = (Wavelength - 510) / (580 - 510);
            Green = 1.0;
            Blue = 0.0;
        }else if((Wavelength >= 580) && (Wavelength<645)){
            Red = 1.0;
            Green = -(Wavelength - 645) / (645 - 580);
            Blue = 0.0;
        }else if((Wavelength >= 645) && (Wavelength<781)){
            Red = 1.0;
            Green = 0.0;
            Blue = 0.0;
        }else{
            Red = 0.0;
            Green = 0.0;
            Blue = 0.0;
        };
        
        // Let the intensity fall off near the vision limits
        
        if((Wavelength >= 380) && (Wavelength<420)){
            factor = 0.3 + 0.7*(Wavelength - 380) / (420 - 380);
        }else if((Wavelength >= 420) && (Wavelength<701)){
            factor = 1.0;
        }else if((Wavelength >= 701) && (Wavelength<781)){
            factor = 0.3 + 0.7*(780 - Wavelength) / (780 - 700);
        }else{
            factor = 0.0;
        };
        
        
        int* rgb = new int[3];
        
        // Don't want 0^x = 1 for x <> 0
        rgb[0] = Red==0.0 ? 0 : (int) (IntensityMax * pow(Red * factor, Gamma));
        rgb[1] = Green==0.0 ? 0 : (int) (IntensityMax * pow(Green * factor, Gamma));
        rgb[2] = Blue==0.0 ? 0 : (int) (IntensityMax * pow(Blue * factor, Gamma));
        
        return Color3f(rgb[0],rgb[1],rgb[2]);
    }
private:
    
};

NORI_REGISTER_CLASS(PathSpectralIntegrator, "path_spectral");
NORI_NAMESPACE_END
