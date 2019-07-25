/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob

    Nori is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    Nori is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <nori/bsdf.h>
#include <nori/frame.h>

NORI_NAMESPACE_BEGIN

/// Ideal dielectric BSDF
class Dielectric : public BSDF {
public:
    Dielectric(const PropertyList &propList) {
        /* Interior IOR (default: BK7 borosilicate optical glass) */
        m_intIOR = propList.getFloat("intIOR", 1.5046f);

        /* Exterior IOR (default: air) */
        m_extIOR = propList.getFloat("extIOR", 1.000277f);
        
        m_albedo = propList.getColor("albedo",1);
    }

    Color3f eval(const BSDFQueryRecord &) const {
        /* Discrete BRDFs always evaluate to zero in Nori */
        return Color3f(0.0f);
    }

    float pdf(const BSDFQueryRecord &) const {
        /* Discrete BRDFs always evaluate to zero in Nori */
        return 0.0f;
    }

    Color3f sample(BSDFQueryRecord &bRec, const Point2f &sample) const {
        Vector3f reflected;
        Vector3f refracted;
        Vector3f normal = Vector3f(0,0,1);
        Vector3f lightIn = -bRec.wi; lightIn.normalize();
        float reflectivity = 0.01;
        float ni = m_intIOR;
        float np = m_extIOR;
        float cosine;
        
        //When Light comes from inside
        if(lightIn.dot(normal) > 0) {
            normal = -normal;
        }
        else
        {
            std::swap(ni, np);
        }
        
        reflectivity = fresnel(-lightIn.dot(normal), np, ni);
        
        reflectivity = (1-ni/np) / (1+ni/np);
        reflectivity = reflectivity * reflectivity;
        cosine = -lightIn.dot(normal);
        reflectivity += (1-reflectivity) * pow((1-cosine),5);
    
        if(!refraction(lightIn, normal, ni/np, bRec.wo))
            reflectivity = 1;
        
        if(sample[0] < reflectivity) {
            reflection(lightIn, normal, bRec.wo);
        }

        bRec.eta = 1.f;
    
        return m_albedo;
    }

    std::string toString() const {
        return tfm::format(
            "Dielectric[\n"
            "  intIOR = %f,\n"
            "  extIOR = %f\n"
            "  albedo = %s\n"
            "]",
            m_intIOR, m_extIOR, m_albedo.toString());
    }
private:
    float m_intIOR, m_extIOR;
    Color3f m_albedo;
};

NORI_REGISTER_CLASS(Dielectric, "dielectric");
NORI_NAMESPACE_END
