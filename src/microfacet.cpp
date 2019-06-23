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
#include <nori/warp.h>

NORI_NAMESPACE_BEGIN

class Microfacet : public BSDF {
public:
    Microfacet(const PropertyList &propList) {
        /* RMS surface roughness */
        m_alpha = propList.getFloat("alpha", 0.1f);

        /* Interior IOR (default: BK7 borosilicate optical glass) */
        m_intIOR = propList.getFloat("intIOR", 1.5046f);

        /* Exterior IOR (default: air) */
        m_extIOR = propList.getFloat("extIOR", 1.000277f);

        /* Albedo of the diffuse base material (a.k.a "kd") */
        m_kd = propList.getColor("kd", Color3f(0.5f));

        /* To ensure energy conservation, we must scale the 
           specular component by 1-kd. 

           While that is not a particularly realistic model of what 
           happens in reality, this will greatly simplify the 
           implementation. Please see the course staff if you're 
           interested in implementing a more realistic version 
           of this BRDF. */
        m_ks = 1 - m_kd.maxCoeff();
    }

    /// Evaluate the BRDF for the given pair of directions
    Color3f eval(const BSDFQueryRecord &bRec) const {
        if (bRec.measure != ESolidAngle
            || Frame::cosTheta(bRec.wi) <= 0
            || Frame::cosTheta(bRec.wo) <= 0)
            return 0.0f;
        
        Vector3f wh = (bRec.wi + bRec.wo); wh.normalize();
        float f = fresnel(wh.dot(bRec.wi), m_extIOR, m_intIOR);
        float cosineI = bRec.wi.z();
        float cosineO = bRec.wo.z();
        float cosineH = wh.z();
        float g = G(bRec.wi, wh) * G(bRec.wo, wh);
        float d = Warp::squareToBeckmannPdf(wh, m_alpha);
        
        return m_kd * INV_PI + m_ks * (d * f * g)/(4 * cosineI * cosineO * cosineH);
    }

    /// Evaluate the sampling density of \ref sample() wrt. solid angles
    float pdf(const BSDFQueryRecord &bRec) const {
        Vector3f wh = (bRec.wi + bRec.wo); wh.normalize();
        float jacobian = 1.f/(4*(wh.dot(bRec.wo)));
        
        float a = m_ks*Warp::squareToBeckmannPdf(wh, m_alpha) * jacobian;
        float b = (1 - m_ks) * bRec.wo.z() * INV_PI;
        
        return a + b;
    }

    /// Sample the BRDF
    Color3f sample(BSDFQueryRecord &bRec, const Point2f &sample) const {
        if(sample[0] < m_ks) {
            //specular case
            const Vector3f n = Warp::squareToBeckmann(Point2f(drand48(), drand48()), m_alpha);
            reflection(-bRec.wi, n, bRec.wo);
        }
        else {
            bRec.wo = Warp::squareToCosineHemisphere(Point2f(drand48(), drand48()));
        }
        // Note: Once you have implemented the part that computes the scattered
        // direction, the last part of this function should simply return the
        // BRDF value divided by the solid angle density and multiplied by the
        // cosine factor from the reflection equation, i.e.
        return eval(bRec) * Frame::cosTheta(bRec.wo) / pdf(bRec);
    }
    
    float G(Vector3f const a, Vector3f const b) const {
        float beta = 1.f/(m_alpha * Frame::tanTheta(a));
        float beta2 = beta * beta;
        if(a.dot(b)/a.z() > 0) {
            if(beta < 1.6) {
                return (3.535*beta + 2.181*beta2)/(1+2.276*beta + 2.577 * beta2);
            }
            else {
                return 1;
            }
        }
        else {
            return 0;
        }
    }

    bool isDiffuse() const {
        /* While microfacet BRDFs are not perfectly diffuse, they can be
           handled by sampling techniques for diffuse/non-specular materials,
           hence we return true here */
        return true;
    }

    std::string toString() const {
        return tfm::format(
            "Microfacet[\n"
            "  alpha = %f,\n"
            "  intIOR = %f,\n"
            "  extIOR = %f,\n"
            "  kd = %s,\n"
            "  ks = %f\n"
            "]",
            m_alpha,
            m_intIOR,
            m_extIOR,
            m_kd.toString(),
            m_ks
        );
    }
private:
    float m_alpha;
    float m_intIOR, m_extIOR;
    float m_ks;
    Color3f m_kd;
};

NORI_REGISTER_CLASS(Microfacet, "microfacet");
NORI_NAMESPACE_END
