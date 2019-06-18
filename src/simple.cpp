//
//  simple.cpp
//  Half
//
//  Created by Junho on 18/06/2019.
//

#include <nori/integrator.h>
#include <nori/scene.h>

#define EPSILON 1.f

NORI_NAMESPACE_BEGIN

class SimpleIntegrator : public Integrator {
public:
    SimpleIntegrator(const PropertyList &props) {
        m_position = props.getPoint("position");
        m_energy = props.getColor("energy");
    }
    
    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
        /* Find the surface that is visible in the requested direction */
        Intersection its;
        if (!scene->rayIntersect(ray, its))
            return Color3f(0.0f);
        
        Point3f hitInWorld = its.p;
        Point3f distanceVector = m_position - hitInWorld;
        float distance = distanceVector.dot(distanceVector);
        distanceVector.normalize();
        Color3f albedo = m_energy / (4 * M_PI * M_PI);
        Ray3f shadowRay = Ray3f(hitInWorld, distanceVector);
        shadowRay.mint = EPSILON;
        float cosine = std::max(0.f, its.shFrame.n.dot(distanceVector));
        
        if(!scene->rayIntersect(shadowRay)) {
            return albedo * cosine / distance;
        }
        
        return Color3f(0.f);
    }
    
    std::string toString() const {
        return "NormalIntegrator[]";
    }
    
private:
    Point3f m_position;
    Color3f m_energy;
};

NORI_REGISTER_CLASS(SimpleIntegrator, "simple");
NORI_NAMESPACE_END
