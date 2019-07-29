//
//  homogeneous.cpp
//  Half
//
//  Created by Junho on 25/07/2019.
//

#include <nori/medium.h>

NORI_NAMESPACE_BEGIN

class HomogeneousMedium : public Medium {
public:
    HomogeneousMedium(const PropertyList &props) {
        sigma_a = props.getColor("sigma_a",0.5);
        sigma_s = props.getColor("sigma_s",0.5);
        sigma_t = sigma_a + sigma_s;
        albedo = sigma_a / sigma_s;
    }
    
    Color3f tr(const Ray3f &ray, const Sampler *sample) const {
        return exp(-sigma_t * std::min(ray.maxt * ray.d.norm(), MAXFLOAT));
    }
    
    Color3f sample(const Ray3f &ray, Sampler &sampler) const {
        return 0.f;
    }
    
    std::string toString() const {
        return "HomogeneousMedium[]";
    }
    
private:
    
};

NORI_REGISTER_CLASS(HomogeneousMedium, "homogeneous");
NORI_NAMESPACE_END
